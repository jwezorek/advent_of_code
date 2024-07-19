#include "../util/util.h"
#include "y2019.h"
#include "intcode.h"
#include <filesystem>
#include <functional>
#include <print>
#include <ranges>
#include <unordered_map>
#include <deque>
#include <boost/functional/hash.hpp>

#ifdef WIN32
#include <Windows.h>
#endif

namespace r = std::ranges;
namespace rv = std::ranges::views;

/*------------------------------------------------------------------------------------------------*/

namespace {

    struct point {
        int x;
        int y;

        bool operator==(const point& p) const {
            return x == p.x && y == p.y;
        }
    };

    point operator+(const point& lhs, const point& rhs) {
        return  { lhs.x + rhs.x, lhs.y + rhs.y };
    }

    point operator-(const point& lhs, const point& rhs) {
        return  { lhs.x - rhs.x, lhs.y - rhs.y };
    }

    point operator*(int lhs, const point& rhs) {
        return {
            lhs * rhs.x,
            lhs * rhs.y
        };
    }

    struct point_hash {
        size_t operator()(const point& pt) const {
            size_t seed = 0;
            boost::hash_combine(seed, pt.x);
            boost::hash_combine(seed, pt.y);
            return seed;
        }
    };

    enum tile {
        empty = 0,
        wall,
        block,
        paddle,
        ball
    };

    using tile_loc_map = std::unordered_map<point, tile, point_hash>;

    int count_blocks(const std::vector<int64_t>& program) {
        aoc::intcode_computer icc(program);
        tile_loc_map screen;

        std::array<int64_t, 3> output_buffer;
        int output_index = 0;
        icc.run(
            []()->int64_t {return 0; },
            [&](int64_t v) {
                output_buffer[output_index++] = v;
                if (output_index == 3) {
                    output_index = 0;
                    point loc = {
                        static_cast<int>(output_buffer[0]),
                        static_cast<int>(output_buffer[1])
                    };
                    screen[loc] = static_cast<tile>(output_buffer[2]);
                }
            }
        );

        return r::fold_left(
            screen | rv::values | rv::transform(
                [](tile t) {
                    return t == block ? 1 : 0;
                }
            ),
            0,
            std::plus<>()
        );
    }

#ifdef WIN32

    // Implemented the game as a turn-based Win32 application that supports
    // undo (backspace) so that it is pretty easy to just play out the 
    // board by undoing as necessary.

    constexpr int k_tile_size = 10;
    constexpr int k_horz_marg = 30;
    constexpr int k_vert_marg = 45;
    constexpr int k_score_y = 10;
    constexpr int k_score_hgt = 35;
    constexpr int k_max_undo = 200;

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    int run_game(const std::vector<int64_t>& program) {

        HINSTANCE hInstance = GetModuleHandle(0);
        MSG msg = { 0 };
        WNDCLASS wc = { 0 };
        wc.lpfnWndProc = WndProc;
        wc.hInstance = hInstance;
        wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
        wc.lpszClassName = "aoc_2019_day_13";

        if (!RegisterClass(&wc))
            return 1;

        static aoc::intcode_computer icc(program);
        icc.value(0) = 2;

        if (!CreateWindow( wc.lpszClassName,  "Advent of Code 2019 - Day 13",
                WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 447, 380, 0, 0,
                hInstance, reinterpret_cast<LPVOID>(&icc)) ) {
            return 2;
        }

        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            DispatchMessage(&msg);
        }

        return 0;
    }

    enum user_input {
        left = -1,
        dont_move = 0,
        right = 1
    };

    class game_state {

        struct frame_contents {
            bool complete;
            tile_loc_map tiles;
            std::optional<int> score;
        };

        struct frame {
            bool complete;
            aoc::intcode_computer pre_input;
            std::optional<aoc::intcode_computer> post_input;
            HBITMAP frame_buffer;

            frame(const aoc::intcode_computer& i, HBITMAP bmp) : 
                complete(false), 
                pre_input(i), 
                frame_buffer(bmp) {
            }

            void release() {
                DeleteObject(frame_buffer);
            }
        };

        std::deque<frame> frames_;
        aoc::intcode_computer initial_;
        int wd;
        int hgt;

        frame_contents generate_next_frame_contents(aoc::intcode_computer& icc) {
            tile_loc_map tiles;
            bool finished_painting = false;
            std::optional<int> score;
            while (!finished_painting) {
                auto result = icc.run_until_event();
                if (result == aoc::terminated) {
                    return { true, tiles, score };
                }
                if (result == aoc::awaiting_input) {
                    finished_painting = true;
                } else {
                    int x = static_cast<int>(icc.output());
                    icc.run_until_event();
                    int y = static_cast<int>(icc.output());
                    icc.run_until_event();
                    int tile_or_score = static_cast<int>(icc.output());

                    if (x == -1 && y == 0) {
                        score = tile_or_score;
                    }
                    else {
                        tiles[{x, y}] = static_cast<tile>(tile_or_score);
                    }
                }
            }
            return { false, tiles, score };
        }

        void paint_tile(HDC hdc, point loc, ::tile tile) {
            int x = k_horz_marg + k_tile_size * loc.x;
            int y = k_vert_marg + k_tile_size * loc.y;

            static std::array<COLORREF, 5> tile_color = { {
                RGB(0,0,0),
                RGB(128,128,128),
                RGB(255,0,0),
                RGB(255,255,255),
                RGB(255,255,255)
            } };

            HBRUSH hbr = CreateSolidBrush(tile_color[static_cast<int>(tile)]);
            RECT r = { x, y, x + k_tile_size, y + k_tile_size };
            if (tile != ball) {
                FillRect(hdc, &r, hbr);
            }
            else {
                auto old_pen = SelectObject(hdc, GetStockObject(NULL_PEN));
                auto old_brush = SelectObject(hdc, hbr);
                Ellipse(hdc, r.left, r.top, r.right, r.bottom);
                SelectObject(hdc, old_brush);
                SelectObject(hdc, old_pen);
            }

            DeleteObject(hbr);
        }

        void paint_score(HDC hdc, int score) {
            std::string score_str = std::to_string(score);
            SetTextColor(hdc, RGB(255, 255, 255));
            SetBkMode(hdc, TRANSPARENT);

            RECT r = { k_horz_marg, k_score_y, k_horz_marg + 500, k_score_y + k_score_hgt };

            FillRect(hdc, &r, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
            DrawText(hdc, score_str.c_str(),
                static_cast<int>(score_str.size()), &r,
                DT_SINGLELINE | DT_LEFT | DT_TOP
            );
        }

        void paint_frame(const frame_contents& frame, HBITMAP frame_buffer, int wd, int hgt) {
            HDC hdc_scr = GetDC(NULL);
            HDC hdc_frame_buffer = CreateCompatibleDC(hdc_scr);
            auto old_bmp = SelectObject(hdc_frame_buffer, frame_buffer);

            for (const auto& [loc, tile] : frame.tiles) {
                paint_tile(hdc_frame_buffer, loc, tile);
            }
            if (frame.score) {
                paint_score(hdc_frame_buffer, *frame.score);
            }

            SelectObject(hdc_frame_buffer, old_bmp);
            DeleteDC(hdc_frame_buffer);
            ReleaseDC(NULL, hdc_scr);
        }

        HBITMAP create_blank_frame_buffer() {
            HDC screen_dc = GetDC(NULL);
            HDC bmp_dc = CreateCompatibleDC(screen_dc);
            
            HBITMAP frame_buffer = CreateCompatibleBitmap(screen_dc, wd, hgt);
            auto old = SelectObject(bmp_dc, frame_buffer);

            RECT r = { 0,0,wd,hgt };
            FillRect(bmp_dc, &r, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));

            SelectObject(bmp_dc, old);
            DeleteDC(bmp_dc);
            ReleaseDC(NULL, screen_dc);

            return frame_buffer;
        }

        HBITMAP clone(HBITMAP src_bmp) {

            HDC screen_dc = GetDC(NULL);
            HDC dst_dc = CreateCompatibleDC(screen_dc);
            HDC src_dc = CreateCompatibleDC(screen_dc);

            HBITMAP dst_bmp = CreateCompatibleBitmap(screen_dc, wd, hgt);
            auto old1 = SelectObject(dst_dc, dst_bmp);
            auto old2 = SelectObject(src_dc, src_bmp);

            BitBlt(dst_dc, 0, 0, wd, hgt, src_dc, 0, 0, SRCCOPY);

            SelectObject(src_dc, old2);
            SelectObject(dst_dc, old1);
            DeleteDC(src_dc);
            DeleteDC(dst_dc);
            ReleaseDC(NULL, screen_dc);

            return dst_bmp;
        }

        void pop_frame() {
            auto bmp = current_frame_buffer();
            frames_.pop_back();
            frames_.back().post_input = {};
            DeleteObject(bmp);
        }

    public:
        game_state(aoc::intcode_computer* c, HWND wnd) : 
                initial_(*c){

            RECT r;
            GetClientRect(wnd, &r);
            wd = r.right - r.left;
            hgt = r.bottom - r.top;

            do_next_turn();
        }

        void do_next_turn() {

            if (!frames_.empty() && frames_.back().complete) {
                return;
            }

            frame new_frame(
                frames_.empty() ? initial_ : current_computer(),
                frames_.empty() ?
                    create_blank_frame_buffer() :
                    clone(current_frame_buffer())
            );

            auto new_contents = generate_next_frame_contents(new_frame.pre_input);
            new_frame.complete = new_contents.complete;

            paint_frame(new_contents, new_frame.frame_buffer, wd, hgt);
            frames_.push_back(new_frame);

            if (frames_.size() > k_max_undo) {
                frames_.pop_front();
            }
        }

        aoc::intcode_computer& current_computer() {
            if (frames_.back().post_input) {
                return *frames_.back().post_input;
            }
            return frames_.back().pre_input;
        }

        HBITMAP current_frame_buffer() {
            return frames_.back().frame_buffer;
        }

        void undo() {
            if (frames_.empty() || frames_.size() == 1) {
                return;
            }
            pop_frame();
        }

        void receive_input(user_input inp) {
            frames_.back().post_input = frames_.back().pre_input;
            auto result = frames_.back().post_input->run_until_event(
                static_cast<int64_t>(inp)
            );
            do_next_turn();
        }

        void render(HDC hdc) {
            HDC hdc_scr = GetDC(NULL);
            HDC hdc_bmp = CreateCompatibleDC(hdc_scr);
            auto old_bmp = SelectObject(hdc_bmp, current_frame_buffer());

            BitBlt(hdc, 0, 0, wd, hgt, hdc_bmp, 0, 0, SRCCOPY);

            SelectObject(hdc_bmp, old_bmp);
            DeleteDC(hdc_bmp);
            ReleaseDC(NULL, hdc_scr);
        }
    };

    game_state* get_game_state(HWND wnd) {
        return reinterpret_cast<game_state*>(
            GetWindowLongPtr(wnd, GWLP_USERDATA)
        );
    }

    struct input_run {
        user_input command;
        int duration;
    };

    LRESULT handle_wm_paint(HWND wnd, WPARAM wparam, LPARAM lparam) {
        auto* state = get_game_state(wnd);

        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(wnd, &ps);
        state->render(hdc);
        EndPaint(wnd, &ps);

        return 0;
    }

    LRESULT handle_wm_keydown(HWND wnd, WPARAM wparam, LPARAM lparam) {
        auto* state = get_game_state(wnd);
        user_input inp = dont_move;
        switch (wparam) {
            case VK_LEFT:
                inp = left;
                break;
            case VK_RIGHT:
                inp = right;
                break;
            case VK_SPACE:
                inp = dont_move;
                break;
            case VK_BACK:
                state->undo();
                InvalidateRect(wnd, NULL, FALSE);
                return 0;
            default:
                return 0;
        }
        state->receive_input(inp);
        InvalidateRect(wnd, NULL, FALSE);
        return 0;
    }

    LRESULT handle_wm_create(HWND wnd, WPARAM wparam, LPARAM lparam) {
        auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);
        auto icc = reinterpret_cast<aoc::intcode_computer*>(cs->lpCreateParams);

        SetWindowLongPtr(wnd, GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(new game_state(icc, wnd))
        );

        InvalidateRect(wnd, NULL, FALSE);
        return 0;
    }

    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message) {
            case WM_CREATE:
                return handle_wm_create(hWnd, wParam, lParam);

            case WM_CLOSE:
                PostQuitMessage(0);
                break;

            case WM_PAINT:
                return handle_wm_paint(hWnd, wParam, lParam);

            case WM_KEYDOWN:
                return handle_wm_keydown(hWnd, wParam, lParam);

            case WM_DESTROY: 
                delete get_game_state(hWnd);
                return 0;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }

#endif
}

void aoc::y2019::day_13(const std::string& title) {

    auto inp = split(
            aoc::file_to_string(aoc::input_path(2019, 13)), ','
        ) | rv::transform(
            [](auto&& str)->int64_t {
                return std::stoi(str);
            }
        ) | r::to<std::vector>();

    std::println("--- Day 13: {} ---", title);
    std::println("  part 1: {}",
        count_blocks(inp)
    );

#ifdef WIN32
    std::println("  part 2: <see game window>" );
    run_game(inp);
#else
    std::println("  part 2: <sorry only a Windows implementation here>");
#endif
    
}