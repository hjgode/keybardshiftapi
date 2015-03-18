========================================================================
    CONSOLE APPLICATION : setKeybdPlane Project Overview
========================================================================

set/unset lock/unlock a keyboard shift plane

The following args are supported:
[setlock|setunlock|set|unset|get] [normal|orange|aqua]

setlock _plane_ 	set and lock plane
setunlock _plane_	unset and unlock plane
set _plane_      	set a plane (no lock)
unset _plane_    	unset a plane
get					get current keyboard plane only, return value is shift+lock
					for example 0x0101 is shift=0x01 and lock=0x01

supported plane names are 'normal', 'orange' and 'aqua'

/////////////////////////////////////////////////////////////////////////////s