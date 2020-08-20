// Copyright (c) 2020 Antero Nousiainen

#ifndef SIGNALS_DISCONNECTABLE_HPP_
#define SIGNALS_DISCONNECTABLE_HPP_

namespace signals
{

    class Disconnectable
    {
    public:
        virtual ~Disconnectable() = default;

        virtual bool connected() const = 0;

        virtual void disconnect() = 0;
    };

}

#endif
