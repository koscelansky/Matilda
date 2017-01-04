#include <type_traits>

#include "board.h"

namespace sc
{
    using namespace detail;

    namespace
    {
        const constexpr Direction operator|(Direction lhs, Direction rhs)
        {
            using T = std::underlying_type_t<Direction>;

            return static_cast<Direction>(static_cast<T>(lhs) | static_cast<T>(rhs));
        }

        const constexpr Direction operator&(Direction lhs, Direction rhs)
        {
            using T = std::underlying_type_t<Direction>;

            return static_cast<Direction>(static_cast<T>(lhs) & static_cast<T>(rhs));
        }

        const constexpr Direction operator<<(Direction lhs, size_t rhs)
        {
            using T = std::underlying_type_t<Direction>;

            return static_cast<Direction>(static_cast<T>(lhs) << rhs);
        }

        constexpr size_t INVALID_POS = std::numeric_limits<size_t>::max();

        size_t get_next_square(size_t origin, Direction direction)
        {
            if (origin < 0 || origin > SQUARES_COUNT)
                throw std::out_of_range("Origin out of bounds.");

            auto rem_by_8 = origin % 8;

            switch (direction)
            {
            case Direction::NE:
            {
                if (origin < 4 || rem_by_8 == 3)
                    return INVALID_POS;
                else if (rem_by_8 >= 4)
                    return origin - 4;
                else
                    return origin - 3;
            }
            case Direction::SE:
            {
                if (origin >= 28 || rem_by_8 == 3)
                    return INVALID_POS;
                else if (rem_by_8 >= 4)
                    return origin + 4;
                else
                    return origin + 5;
            }
            case Direction::SW:
            {
                if (origin >= 28 || rem_by_8 == 4)
                    return INVALID_POS;
                else if (rem_by_8 >= 4)
                    return origin + 3;
                else
                    return origin + 4;
            }
            case Direction::NW:
            {
                if (origin < 4 || rem_by_8 == 4)
                    return INVALID_POS;
                else if (rem_by_8 >= 4)
                    return origin - 5;
                else
                    return origin - 4;
            }
            default:
                throw std::runtime_error("Unknown direction.");
            }
        }

        Direction get_direction(size_t start, size_t end)
        {
            for (const auto& dir : { Direction::NE, Direction::NW, Direction::SE, Direction::SW })
            {
                auto x = start;
                while (true)
                {
                    x = get_next_square(x, dir);

                    if (x == end)
                        return dir;

                    if (x == INVALID_POS)
                        break;
                }
            }

            throw std::runtime_error("The is no straight path between selected points.");
        }

        /**
        * Returns all direction piece can move (in one bit mask).
        */
        Direction get_directions_for_piece(Piece piece)
        {
            if (piece.type() == PieceType::King)
                return Direction::NE | Direction::NW | Direction::SE | Direction::SW;
            else if (piece.color() == PieceColor::Black)
                return Direction::NE | Direction::NW;
            else
                return Direction::SE | Direction::SW;
        }

        Direction get_opposite_direction(Direction dir)
        {
            switch (dir)
            {
            case Direction::NE:
                return Direction::SW;
            case Direction::SE:
                return Direction::NW;
            case Direction::SW:
                return Direction::NE;
            case Direction::NW:
                return Direction::SE;
            }

            throw std::invalid_argument("Invalid direction specified.");
        }
    }


    SlovakCheckersBoard::SlovakCheckersBoard()
    {
        for (auto i = 0u; i < BOARD_SIZE; ++i)
        {
            m_pieces[i] = Piece(PieceColor::White, PieceType::Man);
        }

        for (auto i = m_pieces.size() - BOARD_SIZE; i < m_pieces.size(); ++i)
        {
            m_pieces[i] = Piece(PieceColor::Black, PieceType::Man);
        }

        m_next_moves = get_moves_internal_();
    }

    SlovakCheckersBoard::SlovakCheckersBoard(const std::string & serialized)
    {
        static const std::regex valid_re(R"(^Next:\s*(B|W)\s*Board:\s*(.*)$)");

        std::smatch match;
        if (!std::regex_match(serialized, match, valid_re) || match.size() != 3)
            throw std::invalid_argument("Serialized board is in wrong format.");

        // we can already set next player
        m_next_player = static_cast<PieceColor>(match[1].str()[0]);

        auto board = match[2].str();

        static const std::regex words_re(R"([^\s]+)");
        auto words_begin = std::sregex_iterator(board.begin(), board.end(), words_re);

        for (std::sregex_iterator i = words_begin; i != std::sregex_iterator(); ++i)
        {
            static const std::regex piece_re(R"(([0-9]+):(W|B)(M|K))");

            auto piece = i->str();

            std::smatch piece_match;
            if (!std::regex_match(piece, piece_match, piece_re) || piece_match.size() != 4)
                throw std::invalid_argument("Piece serialization is in wrong format.");

            unsigned pos = std::stoul(piece_match[1].str());
            if (m_pieces[pos])
                throw std::invalid_argument("Piece position redefined.");

            auto color = static_cast<PieceColor>(piece_match[2].str()[0]);
            auto type = static_cast<PieceType>(piece_match[3].str()[0]);
            m_pieces[pos] = Piece(color, type);
        }

        m_next_moves = get_moves_internal_();
    }

    std::string SlovakCheckersBoard::serialize() const
    {
        std::string ret_val("Next: ");
        ret_val += static_cast<char>(m_next_player);

        ret_val += " Board: ";

        for (auto it = m_pieces.cbegin(); it != m_pieces.cend(); ++it)
        {
            if (*it)
            {
                ret_val += std::to_string(std::distance(m_pieces.cbegin(), it));
                ret_val += ':';
                ret_val += static_cast<char>(it->color());
                ret_val += static_cast<char>(it->type());
                ret_val += ' ';
            }
        }

        return ret_val;
    }

    void SlovakCheckersBoard::perform_move(Move move)
    {
        auto active_piece = m_pieces[move.steps().front()];
        m_pieces[move.steps().front()] = Piece();

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

                    if (m_pieces[start])
                    {
                        assert(m_pieces[start].color() != m_next_player);
                        m_pieces[start] = Piece();
                        break;
                    }
                }
            }
        }

        m_pieces[move.steps().back()] = active_piece;

        if (active_piece.type() == PieceType::Man)
        {
            if (active_piece.color() == PieceColor::White)
            {
                if (move.steps().back() >= 28)
                {
                    m_pieces[move.steps().back()] = m_pieces[move.steps().back()].get_promoted();
                }
            }
            else
            {
                if (move.steps().back() < 4)
                {
                    m_pieces[move.steps().back()] = m_pieces[move.steps().back()].get_promoted();
                }
            }
        }

        m_next_player = m_next_player == PieceColor::White ? PieceColor::Black : PieceColor::White;

        // check if game doesn't ended with this move 

        m_next_moves = get_moves_internal_();

        if (m_next_moves.empty())
        {
            bool white_has_pieces = std::any_of(m_pieces.begin(), m_pieces.end(), [](const auto& i)
            {
                return i.color() == PieceColor::White;
            });

            bool black_has_pieces = std::any_of(m_pieces.begin(), m_pieces.end(), [](const auto& i)
            {
                return i.color() == PieceColor::Black;
            });

            if (white_has_pieces && black_has_pieces)
            {
                m_winner = Winner::Draw;
            }
            else
            {
                m_winner = black_has_pieces ? Winner::Black : Winner::White;
            }
        }
    }

    std::vector<Move> SlovakCheckersBoard::get_moves_internal_() const
    {
        std::vector<Move> ret_val;

        ret_val = get_captures_for_type_(PieceType::King);
        if (!ret_val.empty())
            return ret_val;

        ret_val = get_captures_for_type_(PieceType::Man);
        if (!ret_val.empty())
            return ret_val;

        ret_val = get_simple_moves_();

        return ret_val;
    }

    std::vector<std::vector<size_t>> SlovakCheckersBoard::get_captures_rec_(size_t square, Piece piece, std::vector<size_t> enemies, detail::Direction direction) const
    {
        std::vector<std::vector<size_t>> ret_val;

        auto capture_square = square;

        while (true)
        {
            capture_square = get_next_square(capture_square, direction);

            if (capture_square == INVALID_POS || m_pieces[capture_square].color() == piece.color())
                return ret_val;

            if (std::find(enemies.begin(), enemies.end(), capture_square) != enemies.end())
                break;

            if (piece.type() == PieceType::Man)
                return ret_val;
        }

        auto landing_square = capture_square;

        std::vector<std::vector<size_t>> no_more_captures;
        while (true)
        {
            landing_square = get_next_square(landing_square, direction);

            if (landing_square == INVALID_POS)
                break;

            no_more_captures.push_back({ landing_square });

            if (m_pieces[landing_square])
                break; // if no landing square then piece cannot jump

            auto new_enemies = enemies;
            new_enemies.erase(std::find(new_enemies.begin(), new_enemies.end(), capture_square));

            Direction piece_directions = get_directions_for_piece(piece);
            for (Direction new_dir = Direction::Begin; new_dir < Direction::End; new_dir = new_dir << 1)
            {
                if ((piece_directions & new_dir) != new_dir)
                    continue;

                if (new_dir == get_opposite_direction(direction))
                    continue;

                for (auto&& i : get_captures_rec_(landing_square, piece, new_enemies, new_dir))
                {
                    i.insert(i.begin(), landing_square);
                    ret_val.push_back(std::move(i));
                }
            }

            if (piece.type() == PieceType::Man)
                break;
        }

        if (ret_val.empty())
        {
            ret_val = std::move(no_more_captures);
        }

        return ret_val;
    }

    std::vector<Move> SlovakCheckersBoard::get_captures_for_type_(PieceType type) const
    {
        std::vector<size_t> active_pieces, enemies;

        for (auto it = m_pieces.begin(); it != m_pieces.end(); ++it)
        {
            if (!*it)
                continue;

            if (it->color() == m_next_player)
            {
                if (it->type() == type)
                {
                    active_pieces.push_back(std::distance(m_pieces.begin(), it));
                }
            }
            else
            {
                enemies.push_back(std::distance(m_pieces.begin(), it));
            }
        }

        std::vector<std::vector<size_t>> paths;

        for (const auto& i : active_pieces)
        {
            Direction piece_directions = get_directions_for_piece(m_pieces[i]);
            for (Direction new_dir = Direction::Begin; new_dir < Direction::End; new_dir = new_dir << 1)
            {
                if ((piece_directions & new_dir) != new_dir)
                    continue;

                for (auto&& x : get_captures_rec_(i, m_pieces[i], enemies, new_dir))
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

    std::vector<Move> SlovakCheckersBoard::get_simple_moves_() const
    {
        std::vector<size_t> active_pieces;

        for (auto it = m_pieces.begin(); it != m_pieces.end(); ++it)
        {
            if (it->color() == m_next_player)
            {
                active_pieces.push_back(std::distance(m_pieces.begin(), it));
            }
        }

        std::vector<Move> ret_val;

        // possible moves
        for (const auto& i : active_pieces)
        {
            Direction piece_directions = get_directions_for_piece(m_pieces[i]);
            for (Direction new_dir = Direction::Begin; new_dir < Direction::End; new_dir = new_dir << 1)
            {
                if ((piece_directions & new_dir) != new_dir)
                    continue;

                auto next_square = i;
                while (true)
                {
                    next_square = get_next_square(next_square, new_dir);

                    if (next_square == INVALID_POS)
                        break; // end of board

                    if (!m_pieces[next_square]) // check if empty
                    {
                        ret_val.emplace_back(std::vector<size_t>{ i, next_square }, MoveType::SimpleMove);
                    }
                    else
                    {
                        break;
                    }

                    if (m_pieces[i].type() == PieceType::Man)
                        break;
                }
            }
        }

        return ret_val;
    }

    std::ostream& operator<<(std::ostream& lhs, const SlovakCheckersBoard& board)
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
                    if (board.m_pieces[num])
                    {
                        lhs << " " << board.m_pieces[num] << " |";
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
            switch (board.m_winner)
            {
            case Winner::Black:
                lhs << "Black";
                break;

            case Winner::White:
                lhs << "White";
                break;

            case Winner::Draw:
                lhs << "Draw";
                break;
            }
        }
        else
        {
            lhs << "Next player: ";
            lhs << (board.next_player() == PieceColor::Black ? "Black" : "White");
            lhs << "\n\n";
        }

        return lhs;
    }
}