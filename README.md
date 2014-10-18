# Hprose for PHP-CPP

## Introduction

*Hprose* is a High Performance Remote Object Service Engine.

It is a modern, lightweight, cross-language, cross-platform, object-oriented, high performance, remote dynamic communication middleware. It is not only easy to use, but powerful. You just need a little time to learn, then you can use it to easily construct cross language cross platform distributed application system.

*Hprose* supports many programming languages, for example:

* AAuto Quicker
* ActionScript
* ASP
* C++
* Dart
* Delphi/Free Pascal
* dotNET(C#, Visual Basic...)
* Golang
* Java
* JavaScript
* Node.js
* Objective-C
* Perl
* PHP
* Python
* Ruby
* ...

Through *Hprose*, You can conveniently and efficiently intercommunicate between those programming languages.

This project is the implementation of Hprose for PHP-CPP.

## How to install

At first, you should install [PHP-CPP](http://www.php-cpp.com).

But now, The PHP-CPP official version does not support hprose-phpcpp,
so you need to use my modified version:

    git clone https://github.com/andot/PHP-CPP.git
    cd PHP-CPP
    git checkout my_branches

to install it on linux, you can run:

    make && sudo make install

If you want to install it as a static library, you can run:

    make -f Makefile.static && sudo make -f Makefile.static install

If you use Mac OS X, you need to change like this in Makefile.

find **LINKER_FLAGS        =   -shared**  change to **LINKER_FLAGS        =   -shared -undefined dynamic_lookup** then save file

    make && sudo make install

Then, download hprose-phpcpp:

    git clone https://github.com/hprose/hprose-phpcpp.git

edit MakeFile

    cd hprose-phpcpp
    vim MakeFile

if you use linux, run:

    make && sudo make install

That was it!

if you use Mac OS X, you need use:

    make -f Makefile.osx
    sudo make install -f Makefile.osx

to build and install it. then add

    extension=hprose.so

to php.ini by yourself.

## How to use

There is no difference with the [hprose-php](https://github.com/hprose/hprose-php). In fact, you still need to use hprose-php (And you should update it to the lastest version). After installation of this extension, the performance will increase exponentially.