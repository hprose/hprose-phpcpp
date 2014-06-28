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

At first, you should install [PHP-CPP](http://www.php-cpp.com/documentation/install).

Then, if you use linux, run:

    make && sudo make install

That was it!

if you use Mac OS X, you need use:

    make -f Makefile.osx
    sudo make install -f Makefile.osx

to build and install it. then add

    extension=hprose.so

to php.ini by yourself.