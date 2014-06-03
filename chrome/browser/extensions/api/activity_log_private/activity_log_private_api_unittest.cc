// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/memory/scoped_ptr.h"
#include "base/strings/utf_string_conversions.h"
#include "chrome/browser/extensions/activity_log/activity_action_constants.h"
#include "chrome/browser/extensions/api/activity_log_private/activity_log_private_api.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

const char kExtensionId[] = "extensionid";
const char kApiCall[] = "api.call";
const char kArgs[] = "[\"hello\",\"world\"]";

}  // extensions

namespace extensions {

using api::activity_log_private::ExtensionActivity;

typedef testing::Test ActivityLogApiUnitTest;

TEST_F(ActivityLogApiUnitTest, ConvertChromeApiAction) {
  scoped_ptr<base::ListValue> args(new base::ListValue());
  args->Set(0, new base::StringValue("hello"));
  args->Set(1, new base::StringValue("world"));
  scoped_refptr<Action> action(new Action(kExtensionId,
                                          base::Time::Now(),
                                          Action::ACTION_API_CALL,
                                          kApiCall));
  action->set_args(args.Pass());
  scoped_ptr<ExtensionActivity> result = action->ConvertToExtensionActivity();
  ASSERT_EQ(ExtensionActivity::ACTIVITY_TYPE_API_CALL, result->activity_type);
  ASSERT_EQ(kExtensionId, *(result->extension_id.get()));
  ASSERT_EQ(kApiCall, *(result->api_call.get()));
  ASSERT_EQ(kArgs, *(result->args.get()));
}

TEST_F(ActivityLogApiUnitTest, ConvertDomAction) {
  scoped_ptr<base::ListValue> args(new base::ListValue());
  args->Set(0, new base::StringValue("hello"));
  args->Set(1, new base::StringValue("world"));
  scoped_refptr<Action> action(new Action(kExtensionId,
                               base::Time::Now(),
                               Action::ACTION_DOM_ACCESS,
                               kApiCall));
  action->set_args(args.Pass());
  action->set_page_url(GURL("http://www.google.com"));
  action->set_page_title("Title");
  action->mutable_other()->SetInteger(activity_log_constants::kActionDomVerb,
                                      DomActionType::INSERTED);
  action->mutable_other()->SetBoolean(activity_log_constants::kActionPrerender,
                                      false);
  scoped_ptr<ExtensionActivity> result = action->ConvertToExtensionActivity();
  ASSERT_EQ(kExtensionId, *(result->extension_id.get()));
  ASSERT_EQ("http://www.google.com/", *(result->page_url.get()));
  ASSERT_EQ("Title", *(result->page_title.get()));
  ASSERT_EQ(kApiCall, *(result->api_call.get()));
  ASSERT_EQ(kArgs, *(result->args.get()));
  scoped_ptr<ExtensionActivity::Other> other(result->other.Pass());
  ASSERT_EQ(ExtensionActivity::Other::DOM_VERB_INSERTED, other->dom_verb);
  ASSERT_TRUE(other->prerender.get());
}

}  // namespace extensions
