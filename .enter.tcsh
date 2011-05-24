if ( ${?BOOST_INCDIR} == "0" ) then
	setenv BOOST_INCDIR $HOME/src/boost-trunk/boost
	printf '[INFO] Setting BOOST_INCDIR environment variable to %s\n' ${BOOST_INCDIR}
endif
