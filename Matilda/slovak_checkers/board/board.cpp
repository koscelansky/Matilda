#include <type_traits>

#include <boost/algorithm/string.hpp>

#include "board.h"

namespace sc
{
	using namespace detail;

    Board::Board()
        : m_board(board_start)
    {
        m_next_moves = get_moves_internal_();
    }

    Board::Board(const std::string& fen)
        : m_board(board_empty)
    {
		std::vector<std::string> parts;
		boost::split(parts, fen, boost::is_any_of(":"), boost::token_compress_off);

		if (parts.size() != 3)
            throw std::invalid_argument("FEN string is in wrong format.");

		if (parts[0].size() != 1)
			throw std::invalid_argument("Missing current player color in FEN string.");

        // we can already set next player
        m_player = static_cast<Color>(parts[0][0]);
		parts.erase(parts.begin());

		auto fill_pieces = [this](Color player, std::string position_str)
		{
			std::vector<std::string> positions;
			boost::split(positions, position_str, boost::is_any_of(","), boost::token_compress_off); 

			for (auto& i : positions)
			{
				if (i.empty())
					throw std::invalid_argument("Position in FEN is bad.");

				Type type = Type::Man;
				if (i[0] == 'K')
				{
					type = Type::King;
					i.erase(i.begin());
				}
				size_t position = std::stoul(i);

				m_board.SetPiece(position - 1, Piece(player, type));
			}
		};

		for (auto& i : parts)
		{
			if (i[0] == 'W')
			{
				fill_pieces(Color::White, i.substr(1));
			}
			else
			{
				fill_pieces(Color::Black, i.substr(1));
			}
		}

		m_next_moves = get_moves_internal_();
    }

	std::string Board::get_fen() const
	{
		std::string ret_val;

		ret_val += m_player == Color::White ? 'W' : 'B';

		for (const auto& i : { Color::White, Color::Black })
		{
			ret_val += ':';
			ret_val += i == Color::White ? 'W' : 'B';

			// save pieces
			for (size_t j = 0; j < SQUARES_COUNT; ++j)
			{
                Piece piece_j = m_board[j];

				if (piece_j.color() == i)
				{
					if (piece_j.type() == Type::King)
					{
						ret_val += 'K';
					}

					ret_val += std::to_string(j + 1);
					ret_val += ',';
				}
			}

			if (!ret_val.empty())
			{
				ret_val.pop_back();
			}
		}

        return ret_val;
    }

    void Board::perform_move(const Move& move)
    {
		// piece performing the move
        auto active_piece = m_board[move.steps().front()];
        
		// move is not reversible if either man is moved or piece is captured 
		bool is_irreversible = active_piece.type() == Type::Man 
			|| move.type() == MoveType::Jump;

		// piece is moved, so make origin square vacant, if the piece lands on 
		// the same square it doesn't matter, because we add it later to the right 
		// place
		m_board.ResetPiece(move.steps().front());

        if (move.type() == MoveType::Jump)
        {
            for (size_t i = 0; i < move.steps().size() - 1; ++i)
            {
                auto start = move.steps()[i];
                auto end = move.steps()[i + 1];

                auto dir = get_direction(start, end);

                while (true)
                {
                    start = get_next_square(start, dir);

                    if (m_board.IsPieceAt(start))
                    {
                        assert(m_board[start].color() != m_player);
                        m_board.ResetPiece(start);
                        break;
                    }
                }
            }
        }

		// put active piece at the right place 
		auto last_square = move.steps().back();

        m_board.SetPiece(last_square, active_piece);

        if (active_piece.type() == Type::Man)
        {
            if (active_piece.color() == Color::White)
            {
                if (last_square >= 28)
                {
                    m_board.Promote(last_square);
                }
            }
            else
            {
                if (move.steps().back() < 4)
                {
					m_board.Promote(last_square);
                }
            }
        }

		// switch players 
        m_player = opponent(m_player);

        // check if game doesn't ended with this move 

        m_next_moves = get_moves_internal_();

        if (m_next_moves.empty())
        {
            bool white_has_pieces = m_board.HasPieces(Color::White);

            bool black_has_pieces = m_board.HasPieces(Color::Black);

            if (white_has_pieces && black_has_pieces)
            {
                m_result = GameResult::Draw;
            }
            else
            {
                m_result = black_has_pieces ? GameResult::BlackWon : GameResult::WhiteWon;
            }
        }
    }

    std::vector<Move> Board::get_moves_internal_() const
    {
        std::vector<Move> ret_val;

        ret_val = get_captures_for_type(Type::King);
        if (!ret_val.empty())
            return ret_val;

        ret_val = get_captures_for_type(Type::Man);
        if (!ret_val.empty())
            return ret_val;

        ret_val = get_simple_moves_();

        return ret_val;
    }

    std::vector<move_vector> Board::get_captures_rec_(size_t square, Piece piece, BitBoard enemies, detail::Direction direction) const
    {
        std::vector<move_vector> ret_val;

        auto capture_square = square;

        while (true)
        {
            capture_square = get_next_square(capture_square, direction);

            if (capture_square == INVALID_POS || m_board[capture_square].color() == piece.color())
                return ret_val;

			// if there is enemy
            if (enemies.test(capture_square)) 
                break;

            if (piece.type() == Type::Man)
                return ret_val;
        }

        auto landing_square = capture_square;

        std::vector<move_vector> no_more_captures;
        while (true)
        {
            landing_square = get_next_square(landing_square, direction);

            if (landing_square == INVALID_POS)
                break;

            if (m_board.IsPieceAt(landing_square))
                break; // if no landing square then piece cannot jump

			no_more_captures.push_back({ landing_square });

            auto new_enemies = enemies;
            new_enemies.reset(capture_square);

            Directions piece_directions = get_directions_for_piece(piece);
            for (uint32_t j = 0; j < piece_directions.size(); ++j)
            {
				if (!piece_directions.test(j))
					continue; // direction is not set

				Direction new_dir = uint32_to_dir(j);

                if (new_dir == get_opposite_direction(direction))
                    continue;

                for (auto&& i : get_captures_rec_(landing_square, piece, new_enemies, new_dir))
                {
                    i.insert(i.begin(), landing_square);
                    ret_val.push_back(std::move(i));
                }
            }

            if (piece.type() == Type::Man)
                break;
        }

        if (ret_val.empty())
        {
            ret_val = std::move(no_more_captures);
        }

        return ret_val;
    }

    std::vector<Move> Board::get_captures_for_type(Type type) const
    {
        Piece active_piece(m_player, type);

        BitBoard active_pos = m_board.GetPieces(m_player, type);
        BitBoard enemies_pos = m_board.GetPieces(opponent(m_player));

        std::vector<move_vector> paths;

        for (size_t i = 0; i < SQUARES_COUNT; ++i)
        {
            if (!active_pos.test(i))
                continue;

            Directions piece_directions = get_directions_for_piece(active_piece);
			for (uint32_t j = 0; j < piece_directions.size(); ++j)
			{
				if (!piece_directions.test(j))
					continue; // direction is not set

				Direction dir = uint32_to_dir(j);

                for (auto&& x : get_captures_rec_(i, active_piece, enemies_pos, dir))
                {
                    x.insert(x.begin(), i);
                    paths.push_back(std::move(x));
                }
            }
        }

        std::vector<Move> ret_val;
        for (auto& i : paths)
        {
            ret_val.emplace_back(std::move(i), MoveType::Jump);
        }

        return ret_val;
    }

    std::vector<Move> Board::get_simple_moves_() const
    {
        BitBoard active_pos = m_board.GetPieces(m_player);

        std::vector<Move> ret_val;

        // possible moves
        for (size_t i = 0; i < SQUARES_COUNT; ++i)
        {
            if (!active_pos.test(i))
                continue;

            Piece active_piece = m_board[i];

            Directions piece_directions = get_directions_for_piece(active_piece);
			for (uint32_t j = 0; j < piece_directions.size(); ++j)
			{
				if (!piece_directions.test(j))
					continue; // direction is not set

				Direction new_dir = uint32_to_dir(j);

                auto next_square = i;
                while (true)
                {
                    next_square = get_next_square(next_square, new_dir);

                    if (next_square == INVALID_POS)
                        break; // end of board

                    if (!m_board.IsPieceAt(next_square)) // check if empty
                    {
                        ret_val.emplace_back(std::initializer_list<size_t>{ i, next_square }, MoveType::SimpleMove);
                    }
                    else
                    {
                        break;
                    }

                    if (active_piece.type() == Type::Man)
                        break;
                }
            }
        }

        return ret_val;
    }

    std::ostream& operator<<(std::ostream& lhs, const Board& board)
    {
        std::string row_separator = "|";
        for (auto i = 0u; i < detail::BOARD_SIZE; ++i)
        {
            row_separator += "----|";
        }

        bool is_white = true;
        for (auto row = 0u; row < detail::BOARD_SIZE; ++row)
        {
            lhs << row_separator << '\n';

            lhs << '|';
            for (auto col = 0u; col < detail::BOARD_SIZE; ++col)
            {
                size_t num = ((row * detail::BOARD_SIZE) + col) / 2;

                if (!is_white)
                {
                    if (board.m_board[num])
                    {
                        lhs << " " << board.m_board[num] << " |";
                    }
                    else
                    {
                        lhs << " " << std::setw(2) << num + 1 << " |";
                    }
                }
                else
                {
                    lhs << "    |";
                }

                is_white = !is_white;
            }

            is_white = !is_white;
            lhs << '\n';
        }

        lhs << row_separator << "\n\n";

        if (board.game_ended())
        {
            lhs << "Game ended. ";
            switch (board.m_result)
            {
            case GameResult::BlackWon:
                lhs << "Black";
                break;

            case GameResult::WhiteWon:
                lhs << "White";
                break;

            case GameResult::Draw:
                lhs << "Draw";
                break;
            }
        }
        else
        {
            lhs << "Next player: ";
            lhs << (board.next_player() == Color::Black ? "Black" : "White");
            lhs << "\n\n";
        }

        return lhs;
    }
}