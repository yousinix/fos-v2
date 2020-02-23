#!/bin/sh

verbose=false

if [ "x$1" = "x-v" ]
then
	verbose=true
	out=/dev/stdout
	err=/dev/stderr
else
	out=/dev/null
	err=/dev/null
fi

pts=5
timeout=30
preservefs=n

echo_n () {
	# suns can't echo -n, and Mac OS X can't echo "x\c"
	# assume argument has no doublequotes
	awk 'BEGIN { printf("'"$*"'"); }' </dev/null
}

runbochs () {
	# Find the address of the kernel readline function,
	# which the kernel monitor uses to read commands interactively.
	brkaddr=`grep 'readline$' obj/kern/kernel.sym | sed -e's/ .*$//g'`
	#echo "brkaddr $brkaddr"

	# Run Bochs, setting a breakpoint at readline(),
	# and feeding in appropriate commands to run, then quit.
	t0=`date +%s.%N 2>/dev/null`
	(
		# The sleeps are necessary in some Bochs to 
		# make it parse each line separately.  Sleeping 
		# here sure beats waiting for the timeout.
		echo vbreak 0x8:0x$brkaddr
		sleep .5
		echo c
		# EOF will do just fine to quit.
	) | (
		ulimit -t $timeout
		# date
		bochs -q 'display_library: nogui' \
			'parport1: enabled=1, file="bochs.out"' 
		# date
	) >$out 2>$err
	t1=`date +%s.%N 2>/dev/null`
	time=`echo "scale=1; ($t1-$t0)/1" | sed 's/.N/.0/g' | bc 2>/dev/null`
	time="(${time}s)"
}


# Usage: runtest <tagname> <defs> <strings...>
runtest () {
	perl -e "print '$1: '"
	rm -f obj/kern/init.o obj/kern/kernel obj/kern/bochs.img 
	[ "$preservefs" = y ] || rm -f obj/fs/fs.img
	if $verbose
	then
		echo "gmake $2... "
	fi
	gmake $2 >$out
	if [ $? -ne 0 ]
	then
		echo gmake $2 failed 
		exit 1
	fi
	runbochs
	if [ ! -s bochs.out ]
	then
		echo 'no bochs.out'
	else
		shift
		shift
		continuetest "$@"
	fi
}

quicktest () {
	perl -e "print '$1: '"
	shift
	continuetest "$@"
}

stubtest () {
    perl -e "print qq|$1: OK $2\n|";
    shift
    score=`expr $pts + $score`
}

continuetest () {
	okay=yes

	not=false
	for i
	do
		if [ "x$i" = "x!" ]
		then
			not=true
		elif $not
		then
			if egrep "^$i\$" bochs.out >/dev/null
			then
				echo "got unexpected line '$i'"
				if $verbose
				then
					exit 1
				fi
				okay=no
			fi
			not=false
		else
			egrep "^$i\$" bochs.out >/dev/null
			if [ $? -ne 0 ]
			then
				echo "missing '$i'"
				if $verbose
				then
					exit 1
				fi
				okay=no
			fi
			not=false
		fi
	done
	if [ "$okay" = "yes" ]
	then
		score=`expr $pts + $score`
		echo OK $time
	else
		echo WRONG $time
	fi
}

# Usage: runtest1 [-tag <tagname>] <progname> [-Ddef...] STRINGS...
runtest1 () {
	if [ $1 = -tag ]
	then
		shift
		tag=$1
		prog=$2
		shift
		shift
	else
		tag=$1
		prog=$1
		shift
	fi
	runtest1_defs=
	while expr "x$1" : 'x-D.*' >/dev/null; do
		runtest1_defs="DEFS+='$1' $runtest1_defs"
		shift
	done
	runtest "$tag" "DEFS='-DTEST=_binary_obj_user_${prog}_start' DEFS+='-DTESTSIZE=_binary_obj_user_${prog}_size' $runtest1_defs" "$@"
}



score=0

runtest1 hello \
	'.00000000. new env 00001000' \
	'hello, world' \
	'i am environment 00001000' \
	'.00001000. exiting gracefully' \
	'.00001000. free env 00001000' \
	'Destroyed the only environment - nothing more to do!'

# the [00001000] tags should have [] in them, but that's 
# a regular expression reserved character, and i'll be damned if
# I can figure out how many \ i need to add to get through 
# however many times the shell interprets this string.  sigh.

runtest1 buggyhello \
	'.00001000. user_mem_check assertion failure for va 00000001' \
	'.00001000. free env 00001000'

runtest1 evilhello \
	'.00001000. user_mem_check assertion failure for va f0100...' \
	'.00001000. free env 00001000'

runtest1 divzero \
	! '1/0 is ........!' \
	'Incoming TRAP frame at 0xefbfff..' \
        'TRAP frame at 0xf.......' \
	'  trap 0x00000000 Divide error' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	'.00001000. free env 00001000'

runtest1 breakpoint \
	'Welcome to the JOS kernel monitor!' \
	'Incoming TRAP frame at 0xefbfffbc' \
        'TRAP frame at 0xf.......' \
	'  trap 0x00000003 Breakpoint' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	! '.00001000. free env 00001000'

runtest1 softint \
	'Welcome to the JOS kernel monitor!' \
	'Incoming TRAP frame at 0xefbfffbc' \
        'TRAP frame at 0xf.......' \
	'  trap 0x0000000d General Protection' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	'.00001000. free env 00001000'

#runtest1 badsegment \
#	'Incoming TRAP frame at 0xefbfffbc' \
#        'TRAP frame at 0xf.......' \
#	'  trap 0x0000000d General Protection' \
#	'  err  0x0000001c' \
#	'  eip  0x008.....' \
#	'  ss   0x----0023' \
#	'.00001000. free env 00001000'

##
## XXX badsegment no longer works on Bochs 2.2.6; Bochs refuses to produce
## a General Protection fault and instead just panics, with the message:
##
## ========================================================================
## Bochs is exiting with the following message:
## [CPU0 ] fetch_raw_descriptor: LDTR.valid=0
## ========================================================================
##
## Unfortunately, we've had to comment this test out. Please reenable it if 
## Bochs fixes this bug!!
##
stubtest badsegment "(stubbed out due to Bochs 2.2.6 bug)"

runtest1 faultread \
	! 'I read ........ from location 0!' \
	'.00001000. user fault va 00000000 ip 008.....' \
	'Incoming TRAP frame at 0xefbfffbc' \
        'TRAP frame at 0xf.......' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000004' \
	'.00001000. free env 00001000'

runtest1 faultreadkernel \
	! 'I read ........ from location 0xf0100000!' \
	'.00001000. user fault va f0100000 ip 008.....' \
	'Incoming TRAP frame at 0xefbfffbc' \
        'TRAP frame at 0xf.......' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000005' \
	'.00001000. free env 00001000' \

runtest1 faultwrite \
	'.00001000. user fault va 00000000 ip 008.....' \
	'Incoming TRAP frame at 0xefbfffbc' \
        'TRAP frame at 0xf.......' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000006' \
	'.00001000. free env 00001000'

runtest1 faultwritekernel \
	'.00001000. user fault va f0100000 ip 008.....' \
	'Incoming TRAP frame at 0xefbfffbc' \
        'TRAP frame at 0xf.......' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000007' \
	'.00001000. free env 00001000'

runtest1 testbss \
	'Making sure bss works right...' \
	'Yes, good.  Now doing a wild write off the end...' \
	'.00001000. user fault va 00c..... ip 008.....' \
	'.00001000. free env 00001000'



echo "Score: $score/60"

if [ $score -lt 60 ]; then
    exit 1
fi



