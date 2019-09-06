#include <type_traits>
#include <assert.h>

#include <regex> // ECMAScript grammar is far worst then PCRE :(
#include <boost/algorithm/string.hpp>

#include "board.h"

namespace SlovakCheckers
{
	using namespace detail;

    Board::Board(const std::string& fen)
        : m_board(kBoardEmpty)
    {
		// following regex will check if the fen is correct and also extract
		// all of the information, there is just one thing that is not properly 
		// checked (duplicates in pieces positions. so W:W1,1:B1 is valid
		// however it is obviously not, use https://regex101.com/ to debug :)
		//
		// https://regex101.com/r/gOWA7E/2
		const char FEN_PATTERN[] = 
R"#(^(B|W):(B|W)((?:K?(?:[1-9]|[1-2][0-9]|3[0-2]),){0,7}K?(?:[1-9]|[1-2][0-9]|3[0-2]))?:(?!\2)(B|W)((?:K?(?:[1-9]|[1-2][0-9]|3[0-2]),){0,7}K?(?:[1-9]|[1-2][0-9]|3[0-2]))?$)#";

		const std::regex fenRegex(FEN_PATTERN);
		std::smatch match;

		if (!std::regex_match(fen, match, fenRegex))
			throw std::invalid_argument("FEN string is in wrong format.");

		m_player = detail::GetColorFromChar(match[1].str()[0]);

		auto fillPieces = [this](Color player, std::string positionStr)
		{
			if (positionStr.empty())
				return;

			std::vector<std::string> positions;
			boost::split(positions, positionStr, boost::is_any_of(","), boost::token_compress_off); 

			for (auto& i : positions)
			{
				assert(!i.empty());

				Type type = Type::Man;
				if (i[0] == 'K')
				{
					type = Type::King;
					i.erase(i.begin());
				}
				uint8_t position = static_cast<uint8_t>(std::stoul(i)) - 1;
				assert(position >= 0 && position < 32);

				if (m_board.IsPieceAt(position))
					throw std::runtime_error("Duplicate positions specified.");

				m_board.SetPiece(position, Piece(player, type));
			}
		};

		fillPieces(detail::GetColorFromChar(match[2].str()[0]), match[3].str());
		fillPieces(detail::GetColorFromChar(match[4].str()[0]), match[5].str());

		if (m_board == BoardState(kBoardEmpty))
			throw std::runtime_error("Empty board is not allowd from FEN.");
    }

	std::string Board::GetFen() const
	{
		std::string result;

		result += m_player == Color::White ? 'W' : 'B';

		for (const auto& color : { Color::White, Color::Black })
		{
			result += ':';
			result += color == Color::White ? 'W' : 'B';

			// save pieces
			for (uint8_t j = 0; j < SQUARES_COUNT; ++j)
			{
				if (!m_board.IsAt(j, color))
					continue;

                auto piece = m_board[j];

				if (piece.GetType() == Type::King)
				{
					result += 'K';
				}

				result += std::to_string(j + 1);
				result += ',';
			}

			if (!result.empty())
			{
				result.pop_back();
			}
		}

        return result;
    }

	auto Board::GetMoves() const -> BoardMoves
	{
		return
		{
			GetJumpsForType(Type::King),
			GetJumpsForType(Type::Man),
			GetSimpleMoves(),
		};
	}

	void Board::perform_move(const Move& move)
    {
		// piece performing the move
        auto activePiece = m_board[move.GetSteps().front()];

		// piece is moved, so make origin square vacant, if the piece lands on 
		// the same square it doesn't matter, because we add it later to the right 
		// place
		m_board.ResetPiece(move.GetSteps().front());

        if (move.GetType() == MoveType::Jump)
        {
            for (size_t i = 0; i < move.GetSteps().size() - 1; ++i)
            {
                auto start = move.GetSteps()[i];
                auto end = move.GetSteps()[i + 1];

                auto dir = get_direction(start, end);

                while (true)
                {
                    start = get_next_square(start, dir);

                    if (m_board.IsPieceAt(start))
                    {
                        assert(m_board[start].GetColor() != m_player);
                        m_board.ResetPiece(start);
                        break;
                    }
                }
            }
        }

		// put active piece at the right place 
		auto last_square = move.GetSteps().back();

        m_board.SetPiece(last_square, activePiece);

        if (activePiece.GetType() == Type::Man)
        {
            if (activePiece.GetColor() == Color::White)
            {
                if (last_square >= 28)
                {
                    m_board.Promote(last_square);
                }
            }
            else
            {
                if (move.GetSteps().back() < 4)
                {
					m_board.Promote(last_square);
                }
            }
        }

		// switch players 
        m_player = Opponent(m_player);
    }

    std::vector<MoveVector> Board::GetJumpsRecursive(uint8_t square, Piece piece, BitBoard enemies, detail::Direction direction) const
    {
        std::vector<MoveVector> result;

		// find where we can in fact capture piece
        auto captureSquare = square;
        while (true)
        {
            captureSquare = get_next_square(captureSquare, direction);

			// if we are out of board, or we find our own piece, there is no capture this way 
            if (captureSquare == INVALID_POS || m_board.IsAt(captureSquare, piece.GetColor()))
                return result;

			// if there is an enemy
            if (enemies.test(captureSquare)) 
                break;

			// only kings can jump multiple squares
            if (piece.GetType() == Type::Man)
                return result;
        }

		// we need to land somewhere after the capture
        auto landingSquare = captureSquare;

        while (true)
        {
            landingSquare = get_next_square(landingSquare, direction);

            if (landingSquare == INVALID_POS)
                break;

            if (m_board.IsPieceAt(landingSquare))
                break; // if no landing square then piece cannot jump

			result.push_back({ landingSquare });

			// we cannot jump one enemy multiple times
            auto newEnemies = enemies;
            newEnemies.reset(captureSquare);

            Directions pieceDirections = GetDirectionsForPiece(piece);
            for (uint32_t j = 0; j < pieceDirections.size(); ++j)
            {
				if (!pieceDirections.test(j))
					continue; // direction is not set

				Direction newDir = uint32_to_dir(j);

                if (newDir == GetOppositeDirection(direction))
                    continue;

                for (auto&& i : GetJumpsRecursive(landingSquare, piece, newEnemies, newDir))
                {
                    i.insert(i.begin(), landingSquare);
                    result.push_back(std::move(i));
                }
            }

            if (piece.GetType() == Type::Man)
                break;
        }

        return result;
    }

    std::vector<Move> Board::GetJumpsForType(Type type) const
    {
        Piece activePiece(m_player, type);

        BitBoard activePositions = m_board.GetPieces(m_player, type);
        BitBoard enemyPositions = m_board.GetPieces(Opponent(m_player));

        std::vector<MoveVector> paths;

        for (uint8_t i = 0; i < SQUARES_COUNT; ++i)
        {
            if (!activePositions.test(i))
                continue; // we are only interested in pieces that can capture

            Directions pieceDirections = GetDirectionsForPiece(activePiece);
			for (uint32_t j = 0; j < pieceDirections.size(); ++j)
			{
				if (!pieceDirections.test(j))
					continue; // direction is not set

				Direction dir = uint32_to_dir(j);

                for (auto&& x : GetJumpsRecursive(i, activePiece, enemyPositions, dir))
                {
                    x.insert(x.begin(), i);
                    paths.push_back(std::move(x));
                }
            }
        }

        std::vector<Move> result;
        for (auto& i : paths)
        {
            result.emplace_back(std::move(i), MoveType::Jump);
        }

        return result;
    }

    std::vector<Move> Board::GetSimpleMoves() const
    {
        std::vector<Move> ret_val;

        // possible moves
        for (uint8_t i = 0; i < SQUARES_COUNT; ++i)
        {
            if (!m_board.IsAt(i, m_player))
                continue;

            Piece active_piece = m_board[i];

            Directions piece_directions = GetDirectionsForPiece(active_piece);
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
                        ret_val.emplace_back(std::initializer_list<uint8_t>{ i, next_square }, MoveType::SimpleMove);
                    }
                    else
                    {
                        break;
                    }

                    if (active_piece.GetType() == Type::Man)
                        break;
                }
            }
        }

        return ret_val;
    }

	size_t Board::GetStateHash() const
	{
		return std::hash<BoardState>()(m_board) ^ std::hash<Color>()(m_player);
	}

    std::ostream& operator<<(std::ostream& lhs, const Board& board)
    {
        std::string rowSeparator = "|";
        for (auto i = 0u; i < detail::BOARD_SIZE; ++i)
        {
            rowSeparator += "----|";
        }

        bool isWhite = true;
        for (auto row = 0u; row < detail::BOARD_SIZE; ++row)
        {
            lhs << rowSeparator << '\n';

            lhs << '|';
            for (auto col = 0u; col < detail::BOARD_SIZE; ++col)
            {
				uint8_t num = ((row * detail::BOARD_SIZE) + col) / 2;

                if (!isWhite)
                {
                    if (board.m_board.IsPieceAt(num))
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

                isWhite = !isWhite;
            }

            isWhite = !isWhite;
            lhs << '\n';
        }

        lhs << rowSeparator << "\n\n";

        return lhs;
    }
}