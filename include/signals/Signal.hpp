// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_SIGNAL_HPP_
#define SIGNALS_SIGNAL_HPP_

namespace signals
{

    class Signal
    {
    public:
        Signal(const Signal&) = delete;

        Signal& operator=(const Signal&) = delete;
    };

}

#endif
