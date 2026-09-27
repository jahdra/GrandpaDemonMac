#include "FaceAlignment.h"
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>

#define CHECK(expr) do { if (!(expr)) { std::cerr << "FAIL line " << __LINE__ << ": " #expr "\n"; std::exit(1); } } while (false)

struct Point { float x, y; bool operator==(Point const&) const = default; };
struct Size { float width, height; bool operator==(Size const&) const = default; };
struct Rect { Point origin; Size size; bool operator==(Rect const&) const = default; };
struct Frame {
    Rect rect;
    Size originalSize;
    Point trim;
    bool rotated;
    int texture;
    bool operator==(Frame const&) const = default;
};

// A CPU model of the relevant cocos2d 2.x sprite operations (not a renderer).
// The tested setFaceFrame is the production helper, not a duplicate of it.
struct Sprite {
    Size canvas;
    Rect rect{};
    Point trim{}, quadOrigin{};
    Point anchor{0.5f, 0.5f}, position{150.f, 120.f}, scale{1.f, 1.f};
    Point decoration{};
    bool rotated = false;
    bool flipX = false, flipY = false;
    int texture = 0, featureState = 0;
    float opacity = 255.f;
    Size getContentSize() const { return canvas; }
    Rect getTextureRect() const { return rect; }
    bool isTextureRectRotated() const { return rotated; }
    void setTextureRect(Rect value, bool rotation, Size untrimmed) {
        rect = value; rotated = rotation; canvas = untrimmed;
        quadOrigin = {(flipX ? -trim.x : trim.x) + (canvas.width - rect.size.width) / 2.f,
                      (flipY ? -trim.y : trim.y) + (canvas.height - rect.size.height) / 2.f};
    }
    void setDisplayFrame(Frame* frame) {
        trim = frame->trim;
        texture = frame->texture;
        setTextureRect(frame->rect, frame->rotated, frame->originalSize);
    }
    Point world(Point local) const {
        return {position.x + (local.x - anchor.x * canvas.width) * scale.x,
                position.y + (local.y - anchor.y * canvas.height) * scale.y};
    }
    Point imageCenter() const {
        return world({quadOrigin.x + rect.size.width / 2.f, quadOrigin.y + rect.size.height / 2.f});
    }
};

bool close(Point a, Point b) {
    return std::abs(a.x - b.x) < 0.001f && std::abs(a.y - b.y) < 0.001f;
}

int main() {
    // Source PNG dimensions at UHD; the CLI supplies 1/4 and 1/2 variants too.
    constexpr std::array<Size, 6> shortFaces{{{160,190},{160,190},{160,190},{166,190},{166,190},{166,208}}};
    // First prove the old raw frame swap can reproduce the reported offset.
    Frame replacement{{{100,200},{61.5f,61.5f}}, {61.5f,61.5f}, {}, false, 1};
    Sprite broken{{40,47.5f}};
    broken.decoration = {20,23.75f};
    auto before = broken.world(broken.decoration);
    broken.setDisplayFrame(&replacement);
    CHECK(!close(before, broken.world(broken.decoration)));

    for (bool text : {false, true}) for (auto pixels : shortFaces) {
        if (text) pixels = {246,246};
        for (float quality : {0.25f,0.5f,1.f}) for (bool rotated : {false,true}) {
            Size original{pixels.width * quality, pixels.height * quality};
            // Non-zero atlas origin and asymmetric trimming must be preserved.
            Frame frame{{{73,91},{original.width - 4,original.height - 8}}, original, {1,-2}, rotated, 9};
            auto unmodified = frame;
            for (Point anchor : {Point{0.5f,0.5f}, Point{0.25f,0.75f}}) {
                Sprite sprite{{40,47.5f}};
                sprite.anchor = anchor;
                sprite.scale = {0.65f,0.8f}; // compact/scaled UI
                sprite.decoration = {sprite.canvas.width / 2.f + 0.25f, sprite.canvas.height / 2.f - 1.f};
                sprite.opacity = 180.f;
                auto savedCanvas = sprite.canvas;
                auto savedChild = sprite.decoration;
                auto savedWorld = sprite.world(savedChild);
                auto savedPosition = sprite.position;
                for (int update = 0; update < 100; ++update) {
                    sprite.featureState = update % 5;
                    sprite.flipX = update % 2;
                    sprite.flipY = update % 3 == 0;
                    CHECK(grandpa::setFaceFrame(&sprite, &frame));
                    CHECK(sprite.canvas == savedCanvas);
                    CHECK(sprite.decoration == savedChild);
                    CHECK(close(sprite.world(savedChild), savedWorld));
                    CHECK(sprite.position == savedPosition);
                    CHECK(sprite.anchor == anchor);
                    CHECK((sprite.scale == Point{0.65f,0.8f}));
                    CHECK(sprite.opacity == 180.f);
                    CHECK(sprite.featureState == update % 5);
                    CHECK(sprite.rect == frame.rect);
                    CHECK(sprite.texture == frame.texture);
                    CHECK(sprite.rotated == rotated);
                    CHECK(frame == unmodified); // cached frames are shared with Fake Rate previews
                    auto expectedCenter = sprite.world({savedCanvas.width / 2.f + (sprite.flipX ? -frame.trim.x : frame.trim.x),
                                                       savedCanvas.height / 2.f + (sprite.flipY ? -frame.trim.y : frame.trim.y)});
                    CHECK(close(sprite.imageCenter(), expectedCenter));
                }
                Frame vanilla{{{0,0},savedCanvas},savedCanvas,{},false,1};
                sprite.setDisplayFrame(&vanilla);
                CHECK(close(sprite.world(savedChild), savedWorld));
                CHECK(sprite.canvas == savedCanvas);
                CHECK(!grandpa::setFaceFrame(&sprite, static_cast<Frame*>(nullptr)));
                CHECK(sprite.texture == 1);
            }
        }
    }
    CHECK(!grandpa::setFaceFrame(static_cast<Sprite*>(nullptr), &replacement));
    Sprite empty{{0,0}};
    CHECK(!grandpa::setFaceFrame(&empty, &replacement));
    CHECK(empty.texture == 0);
    std::cout << "Face/glow geometry: regression reproduced; canvas, trim, rotation and refresh tests passed.\n";
}
