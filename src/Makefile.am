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
	adj2.h \
	aniparse.h \
	array_print.h \
	arrayplot.h \
	autevd.h \
	autlib1.h \
	autlib2.h \
	autlim.h \
	auto_define.h \
	auto_nox.h \
	auto_x11.h \
	autpp.h \
	axes2.h \
	browse.h \
	calc.h \
	choice_box.h \
	color.h \
	comline.h \
	cv2.h \
	dae_fun.h \
	del_stab.h \
	delay_handle.h \
	derived.h \
	diagram.h \
	dialog_box.h \
	do_fit.h \
	dormpri.h \
	edit_rhs.h \
	eig_list.h \
	eigen.h \
	extra.h \
	fftn.h \
	findsing.h \
	flags.h \
	form_ode.h \
	gear.h \
	ggets.h \
	graf_par.h \
	graphics.h \
	help_defs.h \
	histogram.h \
	homsup.h \
	init_conds.h \
	integrate.h \
	kbs.h \
	kinescope.h \
	load_eqn.h \
	lunch-new.h \
	main.h \
	many_pops.h \
	markov.h \
	matrixalg.h \
	menu.h \
	menudrive.h \
	menus.h \
	my_ps.h \
	my_rhs.h \
	my_svg.h \
	myfonts.h \
	mykeydef.h \
	newhome.h \
	newpars.h \
	nullcline.h \
	numerics.h \
	odesol2.h \
	parserslow.h \
	pop_list.h \
	pp_shoot.h \
	read_dir.h \
	rubber.h \
	run_auto.h \
	scrngif.h \
	simplenet.h \
	stiff.h \
	storage.h \
	strutil.h \
	tabular.h \
	toons.h \
	torus.h \
	tutor.h \
	txtread.h \
	userbut.h \
	volterra2.h \
	xpplim.h \
	$(base_headers) \
	$(bitmap_headers) \
	$(solver_headers) \
	$(ui_x11_headers)

base_headers = \
	base/timeutil.h

bitmap_headers = \
	bitmap/alert.bitmap \
	bitmap/aniwin.bitmap \
	bitmap/array.bitmap \
	bitmap/auto.bitmap \
	bitmap/bc.bitmap \
	bitmap/browse.bitmap \
	bitmap/delay.bitmap \
	bitmap/eqns.bitmap \
	bitmap/equilib.bitmap \
	bitmap/filebrowse.bitmap \
	bitmap/graph.bitmap \
	bitmap/home.bitmap \
	bitmap/ic.bitmap \
	bitmap/info.bitmap \
	bitmap/linedn.bitmap \
	bitmap/lines.bitmap \
	bitmap/lineup.bitmap \
	bitmap/pagedn.bitmap \
	bitmap/pageup.bitmap \
	bitmap/param.bitmap \
	bitmap/pp.bitmap \
	bitmap/start.bitmap \
	bitmap/txtview.bitmap

solver_headers = \
	solver/backeuler.h \
	solver/discrete.h \
	solver/euler.h \
	solver/rk4.h \
	solver/symplect.h

ui_x11_headers = \
	ui-x11/editutil.h \
	ui-x11/file-selector.h

xppaut_LDADD = \
	$(top_builddir)/cvodesrc/libcvode.a \
	$(top_builddir)/libi77/libi77.a

xppaut_SOURCES = \
	abort.c \
	adj2.c \
	aniparse.c \
	array_print.c \
	arrayplot.c \
	axes2.c \
	browse.c \
	calc.c \
	choice_box.c \
	color.c \
	comline.c \
	dae_fun.c \
	del_stab.c \
	delay_handle.c \
	derived.c \
	dialog_box.c \
	do_fit.c \
	dormpri.c \
	edit_rhs.c \
	eig_list.c \
	eigen.c \
	extra.c \
	fftn.c \
	findsing.c \
	flags.c \
	form_ode.c \
	gear.c \
	ggets.c \
	graf_par.c \
	graphics.c \
	histogram.c \
	homsup.c \
	init_conds.c \
	integrate.c \
	kinescope.c \
	load_eqn.c \
	lunch-new.c \
	main.c \
	many_pops.c \
	markov.c \
	matrixalg.c \
	menu.c \
	menudrive.c \
	menus.c \
	my_ps.c \
	my_rhs.c \
	my_svg.c \
	nagroutines.c \
	nullcline.c \
	numerics.c \
	odesol2.c \
	parserslow.c \
	pop_list.c \
	pp_shoot.c \
	read_dir.c\
	rubber.c \
	scrngif.c \
	simplenet.c \
	stiff.c \
	storage.c \
	tabular.c \
	torus.c \
	txtread.c \
	userbut.c \
	volterra2.c \
	$(auto_sources) \
	$(base_sources) \
	$(cvode_sources) \
	$(solver_sources) \
	$(strutil_sources) \
	$(ui_x11_sources)

libxpp_a_SOURCES = \
	abort.c \
	adj2.c \
	aniparse.c \
	array_print.c \
	arrayplot.c \
	axes2.c \
	browse.c \
	calc.c \
	choice_box.c \
	color.c \
	comline.c \
	dae_fun.c \
	del_stab.c \
	delay_handle.c \
	derived.c \
	dialog_box.c \
	do_fit.c \
	dormpri.c \
	edit_rhs.c \
	eig_list.c \
	eigen.c \
	extra.c \
	fftn.c \
	findsing.c \
	flags.c \
	form_ode.c \
	gear.c \
	ggets.c \
	graf_par.c \
	graphics.c \
	histogram.c \
	homsup.c \
	init_conds.c \
	integrate.c \
	kinescope.c \
	load_eqn.c \
	lunch-new.c \
	main.c \
	many_pops.c \
	markov.c \
	matrixalg.c \
	menu.c \
	menudrive.c \
	menus.c \
	my_ps.c \
	my_svg.c \
	nagroutines.c \
	nullcline.c \
	numerics.c \
	odesol2.c \
	parserslow.c \
	pop_list.c \
	pp_shoot.c \
	read_dir.c \
	rubber.c \
	scrngif.c \
	simplenet.c \
	stiff.c \
	storage.c \
	tabular.c \
	torus.c \
	txtread.c \
	userbut.c \
	volterra2.c \
	$(auto_sources) \
	$(base_sources) \
	$(cvode_sources) \
	$(solver_sources) \
	$(strutil_sources) \
	$(ui_x11_sources)

auto_sources = \
	autevd.c \
	autlib1.c \
	autlib2.c \
	autlib3.c \
	auto_nox.c \
	auto_x11.c \
	autpp.c \
	diagram.c \
	flowkm_small.c \
	run_auto.c

base_sources = \
	base/timeutil.c

cvode_sources = \
	cv2.c

solver_sources = \
	solver/backeuler.c \
	solver/discrete.c \
	solver/euler.c \
	solver/rk4.c \
	solver/symplect.c

ui_x11_sources = \
	ui-x11/editutil.c \
	ui-x11/file-selector.c

if HAVE_STRUPR
strutil_sources =
else
strutil_sources = strutil.c
endif
