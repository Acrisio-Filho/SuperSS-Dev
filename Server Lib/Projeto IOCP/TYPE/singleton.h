// Arquivo singleton.h
// Criado em 27/04/2019 as 11:11 por Acrisio
// Definição e implementação da classe template singleton

#pragma once
#ifndef _STDA_SINGLETON_H
#define _STDA_SINGLETON_H

namespace stdA {

    template<class _ST> class Singleton {
        public:
            static _ST& getInstance();

            // delete copy and move constructors and assign operators
            Singleton(Singleton const& ) = delete;              // Copy constructor
            Singleton(Singleton&& ) = delete;                   // Move constructor
            Singleton& operator=(Singleton const& ) = delete;   // Copy assign
            Singleton& operator=(Singleton&& ) = delete;        // Move assign

        protected:
            Singleton() {};     // Constructor
            ~Singleton() {};    // Destructor
    };

    template<class _ST> _ST& Singleton<_ST>::getInstance()  {

        // Só funciona Thread-safe do C++ 11 para cima(mais recente)
        static _ST myInstance;

        return myInstance;
    }
}

#endif // !_STDA_SINGLETON_H