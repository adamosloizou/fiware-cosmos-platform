/*
 * mob_environment_parameters.h
 *
 *  Created on: 23/02/2011
 *      Author: jges
 */

#ifndef MOB_ENVIRONMENT_PARAMETERS_H_
#define MOB_ENVIRONMENT_PARAMETERS_H_

#define MOB_PARAMETER_CONF_TIMESLOT_HOME									"MOB.conf_timeslot_home"
#define MOB_PARAMETER_CONF_TIMESLOT_HOME_DEFAULT							"2|0001111 21:00:00 23:59:59|1100000 08:00:00 12:59:59"

#define MOB_PARAMETER_CONF_TIMESLOT_WORK									"MOB.conf_timeslot_work"
#define MOB_PARAMETER_CONF_TIMESLOT_WORK_DEFAULT							"1|0011111 09:00:00 14:59:59"

#define MOB_PARAMETER_MINMONTHCALLS_IN "MOB.parameter_minMonthCalls_in"
#define MOB_PARAMETER_MINMONTHCALLS_IN_DEFAULT 2

#define MOB_PARAMETER_MINMONTHCALLS_NAT_HOME "MOB.parameter_minMonthCalls_nat_home"
#define MOB_PARAMETER_MINMONTHCALLS_NAT_HOME_DEFAULT 5

#define MOB_PARAMETER_MINMONTHCALLS_STA "MOB.parameter_minMonthCalls_sta"
#define MOB_PARAMETER_MINMONTHCALLS_STA_DEFAULT 11

#define MOB_PARAMETER_MINMONTHCALLS_LAC "MOB.parameter_minMonthCalls_lac"
#define MOB_PARAMETER_MINMONTHCALLS_LAC_DEFAULT 7

#define MOB_PARAMETER_MINPERLACSTACALLS "MOB.parameter_minPerLacStaCalls"
#define MOB_PARAMETER_MINPERLACSTACALLS_DEFAULT 80

#define MOB_PARAMETER_NUMDAYSMIN_CELL_HOME "MOB.parameter_numDaysMin_cell_home"
#define MOB_PARAMETER_NUMDAYSMIN_CELL_HOME_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_CELL_HOME "MOB.parameter_freqMin_cell_home"
#define MOB_PARAMETER_FREQMIN_CELL_HOME_DEFAULT 0.0

#define MOB_PARAMETER_NUMDAYSMIN_BTS_HOME "MOB.parameter_numDaysMin_bts_home"
#define MOB_PARAMETER_NUMDAYSMIN_BTS_HOME_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_BTS_HOME "MOB.parameter_freqMin_bts_home"
#define MOB_PARAMETER_FREQMIN_BTS_HOME_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_LAC_HOME "MOB.numDaysMin_lac_home"
#define MOB_PARAMETER_NUMDAYSMIN_LAC_HOME_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_LAC_HOME "MOB.freqMin_lac_home"
#define MOB_PARAMETER_FREQMIN_LAC_HOME_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_STA_HOME "MOB.numDaysMin_sta_home"
#define MOB_PARAMETER_NUMDAYSMIN_STA_HOME_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_STA_HOME "MOB.freqMin_sta_home"
#define MOB_PARAMETER_FREQMIN_STA_HOME_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_CELL_WORK "MOB.numDaysMin_cell_work"
#define MOB_PARAMETER_NUMDAYSMIN_CELL_WORK_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_CELL_WORK "MOB.freqMin_cell_work"
#define MOB_PARAMETER_FREQMIN_CELL_WORK_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_BTS_WORK "MOB.numDaysMin_bts_work"
#define MOB_PARAMETER_NUMDAYSMIN_BTS_WORK_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_BTS_WORK "MOB.freqMin_bts_work"
#define MOB_PARAMETER_FREQMIN_BTS_WORK_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_LAC_WORK "MOB.numDaysMin_lac_work"
#define MOB_PARAMETER_NUMDAYSMIN_LAC_WORK_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_LAC_WORK "MOB.freqMin_lac_work"
#define MOB_PARAMETER_FREQMIN_LAC_WORK_DEFAULT 0

#define MOB_PARAMETER_NUMDAYSMIN_STA_WORK "MOB.numDaysMin_sta_work"
#define MOB_PARAMETER_NUMDAYSMIN_STA_WORK_DEFAULT 1

#define MOB_PARAMETER_FREQMIN_STA_WORK "MOB.freqMin_sta_work"
#define MOB_PARAMETER_FREQMIN_STA_WORK_DEFAULT 0








#define MOB_FLAG_TRUE								"true"
#define MOB_FLAG_FALSE								"false"

#define MOB_FLAG_ONLY_EXTREME				"MOB.only_extreme"
#define MOB_FLAG_FUSING_COMMUNITIES		"MOB.fusing_communities"
#define MOB_FLAG_NO_DUPLICATE												"MOB.no_duplicate"
#define MOB_FLAG_NO_DUPLICATE_DEFAULT										0
#define MOB_FLAG_NODES											"MOB.customers"
#define MOB_FLAG_PARSE_DIR													"MOB.parse_dir"
#define MOB_FLAG_PARSE_DIR_DEFAULT											0
#define MOB_FLAG_TELEFONICA_NODES											"MOB.telefonica_nodes"
#define MOB_FLAG_TELEFONICA_NODES_DEFAULT											"true"
#define MOB_FLAG_TELEFONICA_NODES_TRUE											"true"
#define MOB_FLAG_TELEFONICA_NODES_FALSE											"true"
#define MOB_FLAG_DUPLICATES														"MOB.duplicates"
#define MOB_FLAG_DUPLICATES_DEFAULT												"false"
#define MOB_FLAG_EXTERN_MARK													"MOB.extern_mark"
#define MOB_FLAG_EXTERN_MARK_DEFAULT											"false"

#endif /* MOB_ENVIRONMENT_PARAMETERS_H_ */
