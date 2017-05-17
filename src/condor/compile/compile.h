// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef COMPILE_H_
#define COMPILE_H_

#include "condor/mem/isolate.h"
#include <string>
#include <fstream>

namespace Condor {
namespace internal{

    class Isolate;

    class Compile{
        Compile(Isolate* isolate);
        ~Compile();

    private:

        Scope* rootScope;
        std::string compiled;
        Isolate* isolate;

    public:
        static Compile* New(Isolate* isolate);
        void Run(Scope* root);
    };

} // namespace internal
}

#endif // COMPILE_H_