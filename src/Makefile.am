AM_CFLAGS = -std=c99 -Wall -pedantic -Werror
AM_CPPFLAGS = \
	-D_XOPEN_SOURCE=600 \
	-DAUTO \
	-DCVODE_YES \
	-DMYSTR1=$(MAJORVER) \
	-DMYSTR2=$(MINORVER) \
	-I$(top_srcdir)/cvodesrc \
	-I$(top_srcdir)/libi77
TEST_LOG_DRIVER = \
	env AM_TAP_AWK='$(AWK)' $(SHELL) \
	$(top_srcdir)/build-aux/tap-driver.sh

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
	dae_fun.h \
	del_stab.h \
	delay_handle.h \
	derived.h \
	diagram.h \
	dialog_box.h \
	do_fit.h \
	edit_rhs.h \
	eig_list.h \
	eigen.h \
	expr_builtins.h \
	extra.h \
	fftn.h \
	findsing.h \
	flags.h \
	form_ode.h \
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
	parserslow.h \
	pop_list.h \
	pp_shoot.h \
	read_dir.h \
	run_auto.h \
	scrngif.h \
	simplenet.h \
	solver.h \
	storage.h \
	strutil.h \
	tabular.h \
	toons.h \
	torus.h \
	tutor.h \
	txtread.h \
	userbut.h \
	xpplim.h \
	$(base_headers) \
	$(bitmap_headers) \
	$(solver_headers) \
	$(ui_x11_headers)

base_headers = \
	base/ndrand.h \
	base/timeutil.h \
	base/vector.h

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
	solver/adams.h \
	solver/backeuler.h \
	solver/cv2.h \
	solver/discrete.h \
	solver/dormpri.h \
	solver/euler.h \
	solver/gear.h \
	solver/modeuler.h \
	solver/rb23.h \
	solver/rk4.h \
	solver/stiff.h \
	solver/symplect.h \
	solver/volterra2.h

ui_x11_headers = \
	ui-x11/edit-box.h \
	ui-x11/editutil.h \
	ui-x11/events.h \
	ui-x11/file-selector.h \
	ui-x11/rubber.h \
	ui-x11/status-bar.h \
	ui-x11/torus-box.h

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
	edit_rhs.c \
	eig_list.c \
	eigen.c \
	expr_builtins.c \
	extra.c \
	fftn.c \
	findsing.c \
	flags.c \
	form_ode.c \
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
	parserslow.c \
	pop_list.c \
	pp_shoot.c \
	read_dir.c\
	scrngif.c \
	simplenet.c \
	solver.c \
	storage.c \
	tabular.c \
	torus.c \
	txtread.c \
	userbut.c \
	$(auto_sources) \
	$(base_sources) \
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
	edit_rhs.c \
	eig_list.c \
	eigen.c \
	expr_builtins.c \
	extra.c \
	fftn.c \
	findsing.c \
	flags.c \
	form_ode.c \
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
	parserslow.c \
	pop_list.c \
	pp_shoot.c \
	read_dir.c \
	scrngif.c \
	simplenet.c \
	solver.c \
	storage.c \
	tabular.c \
	torus.c \
	txtread.c \
	userbut.c \
	$(auto_sources) \
	$(base_sources) \
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
	base/ndrand.c \
	base/timeutil.c \
	base/vector.c

solver_sources = \
	solver/adams.c \
	solver/backeuler.c \
	solver/discrete.c \
	solver/dormpri.c \
	solver/euler.c \
	solver/gear.c \
	solver/modeuler.c \
	solver/rb23.c \
	solver/rk4.c \
	solver/stiff.c \
	solver/symplect.c \
	solver/volterra2.c \
	$(cvode_sources)

cvode_sources = \
	solver/cv2.c

ui_x11_sources = \
	ui-x11/edit-box.c \
	ui-x11/editutil.c \
	ui-x11/events.c \
	ui-x11/file-selector.c \
	ui-x11/rubber.c \
	ui-x11/status-bar.c \
	ui-x11/torus-box.c

if HAVE_STRUPR
strutil_sources =
else
strutil_sources = strutil.c
endif

TESTS = $(check_PROGRAMS)
check_PROGRAMS = base_vector_test

base_vector_test_SOURCES = base/vector_test.c
base_vector_test_CFLAGS = @CHECK_CFLAGS@
base_vector_test_LDADD = $(builddir)/libxpp.a @CHECK_LIBS@
