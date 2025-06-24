//
// Created by cris0 on 5/6/2025.
//

#ifndef DESCRIPTORSTATE_H
#define DESCRIPTORSTATE_H

enum class ParseState {
    NewState,
    CompleteState,
    BoolState,
    KeyState,
    StringState,
    NumberState,
    OpenObjectState,
    CloseObjectState,
    OpenArrayState,
    CloseArrayState,
    Null
};

#endif //DESCRIPTORSTATE_H
