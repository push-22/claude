---
name: system-xml
description: Guide for understanding the layout of a system xml file, use this when answering any questions about journeys/stop location/schedules etc
---

## `The layout of a system xml file`
```XML
<AcisConfiguration type="System" originator="msiczkowski" date="2011-09-01" time="13:55:19" server="." database="CD_CBP_20110822_OffRoute" pips="518">
	<sys id="36" version="20110901" start="" end="">
		<mp id="1" e="393130" n="259271"/>
		<mp id="2" e="393135" n="258776"/>
		<mp id="3" e="393132" n="258313"/>
		<mp id="4" e="393022" n="257875"/>
		<mp id="5" e="392522" n="257808"/>
		<mp id="6" e="392144" n="258131"/>
		<mp id="7" e="392478" n="258734"/>
		<stop id="1" name="Bulstrode Gardens" mp="1" stopcode="0500CCITY001" identifier="" pipid="518"/>
		<stop id="2" name="Northampton Street" mp="2" stopcode="0500CCITY002" identifier="" pipid="47"/>
		<stop id="3" name="Victoria Avenue" mp="3" stopcode="0500CCITY003" identifier=""/>
		<stop id="4" name="Michal Street" mp="4" stopcode="0500CCITY004" identifier=""/>
		<stop id="5" name="Northcote Street" mp="5" stopcode="0500CCITY005" identifier=""/>
		<stop id="6" name="Queens Drive" mp="6" stopcode="0500CCITY006" identifier="" pipid="749"/>
		<stop id="7" name="Leasway" mp="7" stopcode="0500CCITY007" identifier="" pipid="3784"/>
		<stop id="8" name="Bulstrode Gardens R" mp="1" stopcode="0500CCITY008" identifier=""/>
		<lnk id="1" from="1" to="2"/>
		<lnk id="2" from="2" to="3"/>
		<lnk id="3" from="3" to="4"/>
		<lnk id="4" from="4" to="5"/>
		<lnk id="5" from="5" to="6"/>
		<lnk id="6" from="6" to="7"/>
		<lnk id="7" from="7" to="8"/>
		<dr id="1" start="2011-08-02" end="2026-02-20"/>
		<psc id="1" name="PSC1">
			<sv id="100" dir="0" origin="Bulstrode Gardens" lnklist="1">
				<dest name="Northampton Street R" ss="2"/>
			</sv>
			<sv id="101" dir="1" origin="Northampton Street" lnklist="1,2,3,4,5,6,7">
				<dest name="Bulstrode Gardens R" ss="8"/>
			</sv>
		</psc>
		<op id="1" opcode="ST" name="Stagecoach East">
			<pat id="1" svid="100" tim="0,T,0,5,T,5" act="P,S">
				<jny id="1" ref="10" dc="127" time="771" shift="0" rb="1" rbRef="100" drl="1"/>
			</pat>
			<pat id="2" svid="101" tim="0,T,0,5,5,8,8,12,12,15,15,20,20,23,23,28,T,28" act="P,B,B,B,B,B,B,S">
				<jny id="2001" ref="11" dc="127" time="540" shift="0" rb="1" rbRef="100" drl="1"/>
				<jny id="2002" ref="11" dc="127" time="555" shift="0" rb="1" rbRef="100" drl="1"/>

			</pat>
			<veh id="1" fleet="1000" type="LF"/>
		</op>
		<op id="2" opcode="TO2" name="Test Op2"></op>
	</sys>
</AcisConfiguration>
```
This is what's known as a **system.xml/schedule file**. It defines the following:
*	The stops in a system and their location
*	The operators that have services running
*	The journeys an operator will run during a day
*	The date ranges a journey will be active between
*	The days a journey will run on

	
each **jny** is in a pattern (**pat**) that describes the scheduled time between stops in minutes, any schedule waiting at the stop, whether the activity is a Pickup, Setdown or both and also what service (svid) it runs

each **sv** defines the sequence link (**lnk**) identifiers (which are stop to stop movements visited) via the **lnklist** attribute, the origin, and one or more destinations via **dest** child elements 

the **lnklist** holds an array seperated by commas of link ids (**lnk**), each link having a from/to attribute which are the ids of stops (**stop**) and optionally a set of point ids (**mp**) to define the route take between the stops.

each **stop** has a name and a position, encoded as an **easting/northing**

each **jny** has:
	a '**time**' - its start time, which is minutes after midnight
	a '**drl**' (daterange list) that points to dateranges (dr) defining start and end dates for the journey:
	a dayclass (**dc**) whose value defines what days the jny runs on, defined below (by adding the individual day values together)

```C++
typedef enum {
    DC_ANY = 0,
    DC_MON = 1,
    DC_TUE = 2,
    DC_WED = 4,
    DC_THU = 8,
    DC_FRI = 16,
    DC_SAT = 32,
    DC_SUN = 64,
    DC_HOL = 128
} Predictor_DayClass;
```

a **pat** **tim** attribute is a pair of arrive/departure times in minutes, one pair for each stop on route, except when theres a 'T', this just represents that the stop is a timing point and the bus CANNOT leave before its due to depart 

The values in the **tim** attribute are absolute offsets from the journey's start time, not relative durations between stops. code can incorrectly add these absolute offsets together.

operators (**op**) have their own journeys, depots and vehicles (**veh**)

With this schedule in place I can define what journeys are going through (visiting) the stop.

I can also "Mesh" the realtime predictions and cancellations in with the schedule data.

a library written in c# called avlsystemxmllib can parse a system xml, and provide various helpers

Notes:
- All values in a `system.xml` generated and are expected to exist and be valid

## `AvlSystemXmlLib`

`AvlSystemXmlLib` is a small helper library that loads the AVL `system.xml` file and builds fast in-memory indexes over its contents.

It is used by the API for three main purposes:

- **Stop code ↔ PIPID mapping**: translate between the stop code used by RTIG requests and the PIPID used by PG/tilde messages.
  - `TryGetPipidFromStopCode(stopCode, out pipId)`
  - `TryGetStopCodeFromPipid(pipId, out stopCode)`

- **Scheduled departures generation**: derive upcoming scheduled arrivals/departures for a given stop directly from `system.xml`.
  - `BuildUpcomingSchedule(pipId, nowLocal, windowMinutes)` returns a list of `SignScheduledArrival` records.

- **System version timestamp**: expose the `system.xml` version timestamp used when subscribing to PG so PG can avoid re-sending schedule blocks unnecessarily.
  - `SystemVersion`

Notes:
- The library caches parsed indexes on the loaded `XDocument` (via annotations) to avoid repeated expensive XML scans.

