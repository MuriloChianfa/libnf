#!/bin/sh 

#
# Copyright (c) 2013-2023, Tomas Podermanski
#    
# This file is part of libnf.net project.
#
# Libnf is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Libnf is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libnf.  If not, see <http://www.gnu.org/licenses/>.
#
#


NFDUMP_VERSION="1.7.1"
NFDUMP="nfdump-$NFDUMP_VERSION"
NFDUMP_MD5="14000174cadb0b6230ef930f1a8c7c71"
NFDUMP_SRC="$NFDUMP.tar.gz"
NFDUMP_URL="https://github.com/phaag/nfdump/archive/v$NFDUMP_VERSION.tar.gz"

BZIP2_VERSION="1.0.6"
BZIP2="bzip2-$BZIP2_VERSION"
BZIP2_MD5="00b516f4704d4a7cb50a1d97e6e8e15b"
BZIP2_SRC="$BZIP2.tar.gz"
BZIP2_URL="https://netcologne.dl.sourceforge.net/project/bzip2/$BZIP2_SRC"


echo ""
echo "##########################################################"
echo "# STAGE 1.1: getting and patching nfdump sources         #"
echo "##########################################################"

if [ ! -f $NFDUMP_SRC ] ; then 
	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	echo "!!                                                     !!"
	echo "!!  If automatic download fails please get nfdump      !!"
	echo "!!  sources manually                                   !!"
	echo "!!                                                     !!"
	echo "!!  VERSION: $NFDUMP                             !!"
	echo "!!  URL:     $NFDUMP_URL  !!"
	echo "!!  FILE:    $NFDUMP.tar.gz                      !!"
	echo "!!                                                     !!"
	echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
	wget -nv -O $NFDUMP_SRC $NFDUMP_URL || curl -L -o $NFDUMP_SRC $NFDUMP_URL || exit 1
fi 

rm -rf nfdump/ $NFDUMP 

./md5sum.sh $NFDUMP_MD5 $NFDUMP_SRC || exit 1

tar xzf $NFDUMP_SRC || exit 1
mv $NFDUMP nfdump  || exit 1
(cd nfdump && ./bootstrap && ./configure && make clean) || exit 1
if [ ! -f nfdump/README ]; then
	echo > nfdump/README
fi

echo ""
echo "##########################################################"
echo "# STAGE 1.1: fetching BZ2 source codes                   #"
echo "##########################################################"
wget -nv -O $BZIP2_SRC $BZIP2_URL || curl -L -o $BZIP2_SRC $BZIP2_URL || exit 1
rm -rf bzip2/ $BZIP2

./md5sum.sh $BZIP2_MD5 $BZIP2_SRC || exit 1

tar xzf $BZIP2_SRC || exit 1
mv $BZIP2 bzip2  || exit 1


echo ""
echo "##########################################################"
echo "# STAGE 2: preparing configure and nfdump sources        #"
echo "##########################################################"

FILES="src/lib/nffile.c src/lib/nfx.c src/lib/nftree.c src/lib/minilzo.c src/lib/lz4.c \
		src/lib/queue.c src/lib/util.c \
		src/lib/grammar.y src/lib/scanner.l \
		  src/lib/ipconv.c src/lib/output_util.c src/lib/sgregex/sgregex.c"
echo "Creating symlinks for $FILES"
for f in $FILES ; do
	bf=$(basename $f)
	(cd src && rm -f $bf && ln -s ../nfdump/$f && cd ..) || exit 1
done

rm src/grammar.c 2> /dev/null
rm src/scanner.c 2> /dev/null

echo ""
echo "##########################################################"
echo "# STAGE 3: checking definitions of all items in libnf    #"
echo "##########################################################"
echo ""
./check_items_map.pl || exit 1

echo ""
echo "##########################################################"
echo "# STAGE 4: creating final ./configure and Makefiles      #"
echo "##########################################################"
./bootstrap || exit 1

echo ""
echo "##########################################################"
echo "# OK: it seems that all steps went well                  #"
echo "##########################################################"

