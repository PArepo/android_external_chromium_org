// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/chromeos/keyboard_driven_event_rewriter.h"

#include <X11/Xlib.h>

#include "chrome/browser/chromeos/login/user_manager.h"
#include "chrome/browser/chromeos/system/input_device_settings.h"
#include "ui/events/event.h"

namespace chromeos {

namespace {

const int kModifierMask = ui::EF_SHIFT_DOWN;

// Returns true if and only if it is on login screen (i.e. user is not logged
// in) and the keyboard driven flag in the OEM manifest is on.
bool ShouldStripModifiersForArrowKeysAndEnter() {
  if (UserManager::IsInitialized() &&
      !UserManager::Get()->IsUserLoggedIn()) {
    return system::keyboard_settings::ForceKeyboardDrivenUINavigation();
  }

  return false;
}

}  // namespace

KeyboardDrivenEventRewriter::KeyboardDrivenEventRewriter() {}

KeyboardDrivenEventRewriter::~KeyboardDrivenEventRewriter() {}

bool KeyboardDrivenEventRewriter::RewriteIfKeyboardDrivenOnLoginScreen(
    ui::KeyEvent* event) {
  if (!ShouldStripModifiersForArrowKeysAndEnter())
    return false;

  return RewriteEvent(event);
}

bool KeyboardDrivenEventRewriter::RewriteForTesting(ui::KeyEvent* event) {
  return RewriteEvent(event);
}

bool KeyboardDrivenEventRewriter::RewriteEvent(ui::KeyEvent* event) {
  if ((event->flags() & kModifierMask) != kModifierMask)
    return false;

  if (event->key_code() != ui::VKEY_LEFT &&
      event->key_code() != ui::VKEY_RIGHT &&
      event->key_code() != ui::VKEY_UP &&
      event->key_code() != ui::VKEY_DOWN &&
      event->key_code() != ui::VKEY_RETURN &&
      event->key_code() != ui::VKEY_F6) {
    return false;
  }

  XEvent* xev = event->native_event();
  XKeyEvent* xkey = &(xev->xkey);
  xkey->state &= ~(ControlMask | Mod1Mask | ShiftMask);
  event->set_flags(event->flags() & ~kModifierMask);
  event->NormalizeFlags();
  return true;
}

}  // namespace chromeos
