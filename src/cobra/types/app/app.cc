// Copyright 2016 Chase Willden and The CobraLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "app.h"

namespace Cobra {
namespace internal{

	void App::CB(Isolate* isolate, std::string sub){
		Script::RunInternalScript(isolate, AppBytes, "app", sub);
	}

	bool App::Included = false;

} // namespace internal
}