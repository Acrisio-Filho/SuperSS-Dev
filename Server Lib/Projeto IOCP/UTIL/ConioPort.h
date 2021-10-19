// Arquivo ConioPort.h
// Criado em 11/06/2021 as 19:03 por Acrisio
// Porte do kbhit para o linux

#ifndef CONIOPORT_H
#define CONIOPORT_H

#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>

struct _static_initialize_conio {
    public:
        _static_initialize_conio() {

            initialized = false;

            // set _kbhit
            ori = getFlag();

            mod = ori;
            
            // Set terminal modify
            mod.c_lflag &= ~(ICANON | ECHO);
            
            modify();
            
            initialized = true;

            // set atexit
            atexit([]() -> void {

                termios ori;

                tcgetattr(fileno(stdin), &ori);

                ori.c_lflag |= (ICANON | ECHO);

                // reset
                tcsetattr(fileno(stdin), TCSANOW, &ori);
                
                setvbuf(stdin, nullptr, _IOLBF, 0);
            });
        };
        ~_static_initialize_conio() {

            if (initialized) {
                
                // reset
                restore();

                initialized = false;
            }
        };

        void restore() {

            setFlag(ori);

            setvbuf(stdin, nullptr, _IOLBF, 0);
        }

        void modify() {
            
            setFlag(mod);

            setbuf(stdin, nullptr);
        }

    private:
        void setFlag(termios& _flag) {
            tcsetattr(fileno(stdin), TCSANOW, &_flag);
        }

        termios getFlag() {

            termios flag;

            tcgetattr(fileno(stdin), &flag);

            return flag;
        }

    private:
        termios ori, mod;
        bool initialized;
};

// Initialize with main start
static _static_initialize_conio lib_port_conio;

int _kbhit() {

    int bytesWaiting;
    ioctl(fileno(stdin), FIONREAD, &bytesWaiting);
    
    return bytesWaiting;
}

int _getch() {

    unsigned char ch;

    int rt = read(fileno(stdin), &ch, 1);

    return (rt > 0 ? ch : -1);
}

void _putch(char _ch) {

    // restore
    lib_port_conio.restore();
    
    int ret_unused = write(fileno(stdout), &_ch, 1);
    
    lib_port_conio.modify();
}

#endif // !CONIOPORT_H