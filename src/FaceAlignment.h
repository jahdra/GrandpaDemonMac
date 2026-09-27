#pragma once

namespace grandpa {
// CCSprite::setDisplayFrame changes the node's untrimmed content size. On a
// GJDifficultySprite that shifts the anchor's pixel position while leaving its
// feature/glow children in the old local coordinates. Keep the vanilla canvas
// instead: setTextureRect recomputes the drawable quad around that same center,
// preserving the new frame's trim offset, rotation and UVs. Merely calling
// setContentSize afterward would leave the quad in the wrong place.
//
// Do not move/re-parent the children or mutate the shared CCSpriteFrame: featured
// decorations, other mods and future vanilla updates still own that geometry.
// This template lets the exact operation be regression-tested without OpenGL.
template <class Sprite, class Frame>
bool setFaceFrame(Sprite* sprite, Frame* frame) {
    if (!sprite || !frame) return false;
    auto canvas = sprite->getContentSize();
    if (canvas.width <= 0 || canvas.height <= 0) return false;
    sprite->setDisplayFrame(frame);
    auto rect = sprite->getTextureRect();
    sprite->setTextureRect(rect, sprite->isTextureRectRotated(), canvas);
    return true;
}
}
