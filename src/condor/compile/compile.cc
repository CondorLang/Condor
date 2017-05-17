// Copyright 2016 Chase Willden and The CondorLang Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "compile.h"

namespace Condor {
    namespace internal{

        Compile::Compile(Isolate* isolate){
            this->isolate = isolate;
        }
        Compile::~Compile(){}

        Compile* Compile::New(Isolate* isolate){
            void* pt = isolate->GetMemory(sizeof(Compile));
            Compile* p = new(pt) Compile(isolate);
            return p;
        }

        void Compile::Run(Scope *root) {
            rootScope = root;
            this->compiled = root->ToString(0);
//            printf("%s", this->compiled.c_str());
            std::ofstream out("compiled.cb.d");
            out << this->compiled;
            out.close();
        }

    } // namespace internal
}