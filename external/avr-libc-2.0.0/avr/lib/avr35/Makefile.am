# Copyright (c) 2004,  Theodore A. Roth
# Copyright (c) 2005, 2008, 2009  Anatoly Sokolov
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in
#   the documentation and/or other materials provided with the
#   distribution.
# * Neither the name of the copyright holders nor the names of
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

SUBDIRS =  at90usb82 at90usb162 ata5505 ata6617c ata664251 atmega8u2 atmega16u2 atmega32u2 attiny167 attiny1634

AVR_TARGET          = avr35
AVR_TARGET_DEFS     = -D__COMPILING_AVR_LIBC__
AVR_TARGET_CFLAGS   = -mcall-prologues -Os
AVR_TARGET_ASFLAGS  = 
AVR_INSTALL_DIR     = avr35

VPATH = $(top_srcdir)/libc/stdlib:$(top_srcdir)/libc/pmstring:$(top_srcdir)/libc/string:$(top_srcdir)/libc/misc:$(top_srcdir)/libc/stdio:$(top_srcdir)/libm/fplib:$(top_srcdir)/libc/time

if HAS_avr35

AVRLIB_DEVLIST =
if HAS_at90usb82
  AVRLIB_DEVLIST += at90usb82
endif	# at90usb82
if HAS_at90usb162
  AVRLIB_DEVLIST += at90usb162
endif	# at90usb162
if HAS_ata5505
  AVRLIB_DEVLIST += ata5505
endif	# ata5505
if HAS_ata6617c
  AVRLIB_DEVLIST += ata6617c
endif	# ata6617c
if HAS_ata664251
  AVRLIB_DEVLIST += ata664251
endif	# ata664251
if HAS_atmega8u2
  AVRLIB_DEVLIST += atmega8u2
endif	# atmega8u2
if HAS_atmega16u2
  AVRLIB_DEVLIST += atmega16u2
endif	# atmega16u2
if HAS_atmega32u2
  AVRLIB_DEVLIST += atmega32u2
endif	# atmega32u2
if HAS_attiny167
  AVRLIB_DEVLIST += attiny167
endif	# attiny167
if HAS_attiny1634
  AVRLIB_DEVLIST += attiny1634
endif	# attiny1634

AM_CPPFLAGS = -I$(top_srcdir)/common -I$(top_srcdir)/include -I$(top_builddir)/include

AVRLIB_CFLAGS = @CDEBUG@ -Wall -W -Wstrict-prototypes -mmcu=$(AVR_TARGET) $(AVR_TARGET_DEFS) $(AVR_TARGET_CFLAGS)

AVRLIB_ASFLAGS = -x assembler-with-cpp @ASDEBUG@ -mmcu=$(AVR_TARGET) $(AVR_TARGET_DEFS) $(AVR_TARGET_ASFLAGS)

AM_CFLAGS = $(AVRLIB_CFLAGS)
AM_CCASFLAGS = $(AM_CPPFLAGS) $(AVRLIB_ASFLAGS)

__install_dir = $(prefix)/avr/lib/$(AVR_INSTALL_DIR)

avrdir = $(__install_dir)
avrlibdir = $(__install_dir)

avr_LIBRARIES = \
	libc.a \
	libprintf_min.a \
	libprintf_flt.a \
	libscanf_min.a \
	libscanf_flt.a \
	libm.a

include $(top_srcdir)/libc/stdlib/Rules.am
include $(top_srcdir)/libc/pmstring/Rules.am
include $(top_srcdir)/libc/string/Rules.am
include $(top_srcdir)/libc/misc/Rules.am
include $(top_srcdir)/libc/stdio/Rules.am
include $(top_srcdir)/libm/fplib/Rules.am
include $(top_srcdir)/libc/time/Rules.am

nodist_libc_a_SOURCES = \
	$(stdlib_a_c_sources) \
	$(stdlib_a_asm_sources) \
	$(pmstring_a_c_sources) \
	$(pmstring_a_asm_sources) \
	$(string_a_c_sources) \
	$(string_a_asm_sources) \
	$(misc_a_c_sources) \
	$(misc_a_asm_sources) \
	$(stdio_a_c_sources) \
	$(stdio_a_asm_sources) \
	$(libm_a_c_sources) \
	$(libm_a_asm_sources) \
	$(time_a_c_sources) \
	$(time_a_asm_sources)

nodist_libm_a_SOURCES = \
	$(libm_a_c_sources) \
	$(libm_a_asm_sources)

libc_a_LIBADD = \
	$(stdio_a_libadd) \
	$(misc_a_libadd)

libc_a_DEPENDENCIES = \
	$(stdio_a_libadd) \
	$(misc_a_libadd)

else

echo all distdir install installdirs clean distclean uninstall check:

endif
