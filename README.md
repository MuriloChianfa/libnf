
[![Maintained by NetX Networks](https://img.shields.io/badge/maintained%20by-NetX%20Networks-blue)](https://netx.as/)

libnf - C interface for processing nfdump files 


IMPORTANT LINKS
===============================================

Official homepage:
	http://libnf.net

C API interface documentation: 
	http://libnf.net/doc/api/

Package directory: 
	http://libnf.net/packages/

Github repository:
	https://github.com/NETX-AS/libnf

Libnf interface for perl (Net::NfDump package) available via cpan:
	http://search.cpan.org/~tpoder/Net-NfDump/lib/Net/NfDump.pm
	https://github.com/netx-as/libnf/tree/master/perl


INSTALLATION FROM GIT REPOSITORY
===============================================

1. Get libnf git repository
```
# git clone https://github.com/MuriloChianfa/libnf
```

2. Go into libnf source directory
```
# cd libnf
```

3. Fetch nfdump sources an prepare for using in libnf
```
# ./prepare-nfdump.sh
```

4. Run configure script
```
# ./configure
```

5. Compile libnf lib
```
# src
# make libnf.la
# cd ..
```

6. Run make and install files
```
# make && make install
```

7. Look at the examples directory how to use libnf
