#!/bin/sh

LC_ALL=C
export LC_ALL

test -z "$srcdir" && srcdir=.
stat=0


if which nm 2>/dev/null >/dev/null; then
	:
else
	echo "check-internal-symbols.sh: 'nm' not found; skipping test"
	exit 77
fi

so=.libs/libharfbuzz.so
if test -f "$so"; then
	echo "Checking that we are exposing internal symbols"
	if nm $so | grep ' T ' | grep -v ' T _fini\>\| T _init\>\| T hb_'; then
		echo "Ouch, internal symbols exposed"
		stat=1
	fi
else
	echo "check-internal-symbols.sh: libharfbuzz.so not found; skipping test"
	exit 77
fi

exit $stat
