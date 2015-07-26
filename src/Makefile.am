AM_CFLAGS = -std=c99 -Wall -pedantic -Werror
AM_CPPFLAGS = \
	-D_XOPEN_SOURCE=600 \
	-DAUTO \
	-DCVODE_YES \
	-DMYSTR1=$(MAJORVER) \
	-DMYSTR2=$(MINORVER) \
	-I$(top_srcdir)/cvodesrc \
	-I$(top_srcdir)/libi77

bin_PROGRAMS = \
	xppaut

lib_LIBRARIES = \
	libxpp.a

noinst_HEADERS = \
	abort.h \
	comline.h \
	extra.h \
	kinescope.h \
	newhome.h \
	stiff.h \
	adj2.h \
	newpars.h \
	storage.h \
	fftn.h \
	nullcline.h \
	struct.h \
	aniparse.h \
	numerics.h \
	arrayplot.h \
	flags.h \
	load_eqn.h \
	odesol2.h \
	tabular.h \
	array_print.h \
	toons.h \
	autevd.h \
	dae_fun.h \
	form_ode.h \
	torus.h \
	autlib1.h \
	delay_handle.h \
	lunch-new.h \
	txtread.h \
	autlib2.h \
	del_stab.h \
	gear.h \
	macdirent.h \
	parserslow.h \
	autlim.h \
	getvar.h \
	macsysdirent.h \
	userbut.h \
	auto_define.h \
	derived.h \
	ggets.h \
	main.h \
	pop_list.h \
	graf_par.h \
	many_pops.h \
	pp_shoot.h \
	auto_nox.h \
	diagram.h \
	graphics.h \
	markov.h \
	volterra2.h \
	auto_x11.h \
	dialog_box.h \
	help_defs.h \
	menudrive.h \
	read_dir.h \
	volterra.h \
	axes2.h \
	do_fit.h \
	histogram.h \
	menu.h \
	rubber.h \
	dormpri.h \
	homsup.h \
	menus.h \
	run_auto.h \
	browse.h \
	myfonts.h \
	scrngif.h \
	calc.h \
	edit_rhs.h \
	init_conds.h \
	mykeydef.h \
	choice_box.h \
	eig_list.h \
	integrate.h \
	my_pars.h \
	shoot.h \
	xpplim.h \
	my_ps.h \
	simplenet.h \
	color.h \
	kbs.h \
	my_rhs.h \
	cv2.h \
	my_svg.h \
	strutil.h \
	tutor.h \
	$(bitmap_headers)

bitmap_headers = \
	bitmap/aniwin.bitmap \
	bitmap/alert.bitmap \
	bitmap/bc.bitmap \
	bitmap/eqns.bitmap \
	bitmap/info.bitmap \
	bitmap/ic.bitmap \
	bitmap/pp.bitmap \
	bitmap/array.bitmap \
	bitmap/browse.bitmap \
	bitmap/equilib.bitmap \
	bitmap/lines.bitmap \
	bitmap/txtview.bitmap \
	bitmap/auto.bitmap \
	bitmap/delay.bitmap \
	bitmap/graph.bitmap \
	bitmap/param.bitmap \
	bitmap/filebrowse.bitmap \
	bitmap/home.bitmap \
	bitmap/linedn.bitmap \
	bitmap/lineup.bitmap \
	bitmap/pagedn.bitmap \
	bitmap/pageup.bitmap \
	bitmap/start.bitmap

xppaut_LDADD = \
	$(top_builddir)/cvodesrc/libcvode.a \
	$(top_builddir)/libi77/libi77.a

xppaut_SOURCES = \
	main.c \
	ggets.c \
	menu.c \
	menus.c \
	rubber.c \
	derived.c \
	many_pops.c \
	pop_list.c \
	graphics.c \
	dialog_box.c \
	numerics.c \
	choice_box.c \
	color.c \
	init_conds.c \
	browse.c \
	kinescope.c \
	axes2.c \
	abort.c \
	parserslow2.c \
	storage.c \
	load_eqn.c \
	form_ode.c \
	odesol2.c \
	gear.c \
	eig_list.c \
	integrate.c \
	delay_handle.c \
	graf_par.c \
	dormpri.c \
	my_ps.c \
	my_svg.c \
	nullcline.c \
	torus.c \
	pp_shoot.c \
	lunch-new.c \
	calc.c \
	adj2.c \
	my_rhs.c \
	read_dir.c\
	volterra2.c \
	tabular.c \
	markov.c \
	histogram.c \
	comline.c \
	edit_rhs.c \
	do_fit.c \
	flags.c \
	del_stab.c \
	stiff.c \
	arrayplot.c \
	array_print.c \
	aniparse.c \
	simplenet.c \
	dae_fun.c \
	fftn.c \
	extra.c \
	scrngif.c \
	nagroutines.c \
	homsup.c \
	txtread.c \
	menudrive.c \
	$(strutil_sources) \
	userbut.c \
	$(cvode_sources) \
	$(auto_sources)

libxpp_a_SOURCES = \
	main.c \
	ggets.c \
	menu.c \
	menus.c \
	rubber.c \
	derived.c \
	many_pops.c \
	pop_list.c \
	graphics.c \
	dialog_box.c \
	numerics.c \
	choice_box.c \
	color.c \
	init_conds.c \
	browse.c \
	kinescope.c \
	axes2.c \
	abort.c \
	parserslow2.c \
	storage.c \
	load_eqn.c \
	form_ode.c \
	odesol2.c \
	gear.c \
	eig_list.c \
	integrate.c \
	delay_handle.c \
	graf_par.c \
	dormpri.c \
	my_ps.c \
	my_svg.c \
	nullcline.c \
	torus.c \
	pp_shoot.c \
	lunch-new.c \
	calc.c \
	adj2.c \
	read_dir.c \
	volterra2.c \
	tabular.c \
	markov.c \
	histogram.c \
	comline.c \
	edit_rhs.c \
	do_fit.c \
	flags.c \
	del_stab.c \
	stiff.c \
	arrayplot.c \
	array_print.c \
	aniparse.c \
	simplenet.c \
	dae_fun.c \
	fftn.c \
	extra.c \
	scrngif.c \
	nagroutines.c \
	homsup.c \
	txtread.c \
	menudrive.c \
	$(strutil_sources) \
	userbut.c \
	$(cvode_sources) \
	$(auto_sources)

auto_sources = \
	autlib1.c \
	autlib2.c \
	autlib3.c \
	autevd.c \
	run_auto.c \
	autpp.c \
	diagram.c \
	auto_nox.c \
	auto_x11.c \
	flowkm_small.c

cvode_sources = \
	cv2.c

if HAVE_STRUPR
strutil_sources =
else
strutil_sources = strutil.c
endif
