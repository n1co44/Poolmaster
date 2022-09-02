<h2>PoolMaster 5.0.0</h2>
<h2>Système de régulation Ph/Orp (Chlore) Arduino Mega2560 (ou Controllino-Maxi) pour piscines domestiques</h2>
Ce projet PoolMaster est basé sur celui de Loic74650 avec quelques modifications (https://github.com/Loic74650/PoolMaster)

Améliorations: (En cours de dévellopement)

    - Pesée des Bidons pour plus de précision (Balance Bidon Chlore et PH)
    - Températures du Local et Exterieur
    - Controle de la PAC via bus RS485 (POOLEX JetLine90)
    - Relevé Conso PAC, Pompe Filtration, Pompe de relevage
    - Relevé Conso Eau
    - Niveau d'eau Puit de décompression
    - Interface Nextion en Français
       
    


<br />
<p align="center"> <img src="/docs/Plan arduino.png" width="802" title="Overview"> </p> <br /><br />



<br />
<p align="center"> <img src="/docs/Page1.png" width="400" title="Page1Nextion 3.5" touch screen"> </p> <br /><br />
<br />
<p align="center"> <img src="/docs/Page2.png" width="400" title="Page2Nextion 3.5" touch screen"> </p> <br /><br />
<br />
<p align="center"> <img src="/docs/Page3.png" width="400" title="Page3Nextion 3.5" touch screen"> </p> <br /><br />
<br />
<p align="center"> <img src="/docs/Page4.png" width="400" title="Page4Nextion 3.5" touch screen"> </p> <br /><br />
<br />
<p align="center"> <img src="/docs/Page5.png" width="400" title="Page5Nextion 3.5" touch screen"> </p> <br /><br />
<br />
<p align="center"> <img src="/docs/PageCalibrationBidon.png" width="400" title="PageCalibNextion 3.5" touch screen"> </p> <br /><br />
<br />
<p align="center"> <img src="/docs/PageCalibrationPH.png" width="400" title="PageCalibPHNextion 3.5" touch screen"> </p> <br /><br />

<h4>Bref description</h4>
	
<p>Quatre mesures principales sont mesurées et rapportées périodiquement sur MQTT et un écran tactile Nextion de 3,5" : température et pression de l'eau, valeurs de pH et d'ORP.<br />
Les états des pompes, les estimations des niveaux des réservoirs et d'autres paramètres sont également signalés périodiquement<br />
Deux boucles de régulation PID fonctionnent en parallèle : une pour le PH, une pour l'ORP<br />
Une boucle de régulation supplémentaire simple (marche/arrêt) gère la température de l'eau (elle démarre/arrête le circulateur du système de chauffage domestique qui apporte de la chaleur à un échangeur de chaleur monté sur les conduites d'eau de la piscine)<br />
Le pH est régulé en injectant de l'acide d'un réservoir dans l'eau de la piscine (un relais démarre/arrête la pompe péristaltique de l'acide)<br />
L'ORP est régulé en injectant du Chlore d'un réservoir dans l'eau de la piscine (un relais démarre/arrête la pompe péristaltique de Chlore)<br />
Des plages horaires et une température d'eau définies permettent de démarrer/arrêter la pompe de filtration pendant une durée quotidienne donnée (un relais démarre/arrête la pompe de filtration)<br />
Les niveaux des réservoirs sont estimés en fonction du temps de fonctionnement et du débit de chaque pompe.<br />
Les paramètres de connectivité Ethernet peuvent être définis via une page Web accessible depuis le réseau local à l'adresse http://PoolMaster.local.<br />
Si une connexion Ethernet est disponible, l'horloge interne (RTC) est synchronisée avec un serveur de temps tous les jours à minuit.<br />

Une fonction API permet d'indiquer au système la température de l'air extérieur. Dans le cas où elle est inférieure à -2,0°C, la filtration est démarrée jusqu'à ce qu'elle remonte au-dessus de +2,0°C<br />
	
La communication avec le système est effectuée à l'aide du protocole MQTT via une connexion Ethernet au réseau local/courtier MQTT.<br /><br />

Toutes les 30 secondes (par défaut), le système publiera sur les "PoolTopicMeas1" et "PoolTopicMeas2"(voir dans le code ci-dessous) les payloads suivants au format Json :<br />
  {"Tmp":818,"pH":321,"PSI":56,"Orp":583,"FilUpT":8995,"PhUpT":0,"ChlUpT":0}<br />
  {"AcidF":100,"ChlF":100,"IO":11,"IO2":0}<br />
  Tmp: measured Water temperature value in °C x100 (8.18°C in the above example payload)<br />
  pH: measured pH value x100 (3.21 in the above example payload)<br />
  Orp: measured Orp (aka Redox) value in mV (583mV in the above example payload)<br />
  PSI: measured Water pressure value in bar x100 (0.56bar in the above example payload)<br />
  FiltUpT: current running time of Filtration pump in seconds (reset every 24h. 8995secs in the above example payload)<br />
  PhUpT: current running time of Ph pump in seconds (reset every 24h. 0secs in the above example payload)<br />
  ChlUpT: current running time of Chl pump in seconds (reset every 24h. 0secs in the above example payload)<br />
  AcidF: percentage fill estimate of acid tank ("pHTank" command must have been called when a new acid tank was set in place in order to have accurate value)<br />
  ChlF: percentage fill estimate of Chlorine tank ("ChlTank" command must have been called when a new Chlorine tank was set in place in order to have accurate value)<br />
IO: a variable of type BYTE where each individual bit is the state of a digital input on the Arduino. These are :<br />	
<ul>
<li>FiltPump: current state of Filtration Pump (0=on, 1=off)</li>
<li>PhPump: current state of Ph Pump (0=on, 1=off)</li>
<li>ChlPump: current state of Chl Pump (0=on, 1=off)</li>
<li>PhlLevel: current state of Acid tank level (0=empty, 1=ok)</li>
<li>ChlLevel: current state of Chl tank level (0=empty, 1=ok)</li>
<li>PSIError: over-pressure error</li>
<li>pHErr: pH pump overtime error flag</li>
<li>ChlErr: Chl pump overtime error flag</li>
</ul><br />
IO2: a variable of type BYTE where each individual bit is the state of a digital input on the Arduino. These are :<br /><br />
<ul>
<li>pHPID: current state of pH PID regulation loop (1=on, 0=off)</li>
<li>OrpPID: current state of Orp PID regulation loop (1=on, 0=off)</li>
<li>Mode: state of pH and Orp regulation mode (0=manual, 1=auto)</li>
<li>Heat: state of water heat command (0=off, 1=on)</li>
<li>R1: state of Relay1 (0=off, 1=on)</li>
<li>R2: state of Relay2 (0=off, 1=on)</li>
<li>R6: state of Relay6 (0=off, 1=on)</li>
<li>R7: state of Relay7 (0=off, 1=on)</li>
</ul><br />

<h4>Comment compiler</h4>
<p>
- ce code a été développé pour deux configurations matérielles principales (voir la liste dans la section matériel ci-dessous) mais avec les options supplementaires, il est préférable avec un Mega 2560:<br /> 
<ul>
<li>Controllino-Maxi or</li> 
<li>Arduino Mega 2560 + Ethernet shield + relay shield + RTC module</li></ul>
- sélectionnez le type de carte cible dans l'IDE Arduino (soit "Arduino Mega 2560" ou "Controllino Maxi") le code doit être compilé pour les deux types<br />


<h4>Compatibilité</h4>
	
<p>Pour que cette esquisse fonctionne sur votre configuration, vous devez modifier les éléments suivants dans le code (dans le fichier "Config.h") :<br />
- éventuellement les définitions de brochage en fonction de votre câblage<br />
- l'adresse unique des capteurs de température d'eau, local et exterieur DS18b20<br />
- Adresse MAC et IP du shield Ethernet<br />
- Adresse IP du courtier MQTT et identifiants de connexion<br />
- possibly the topic names on the MQTT broker to subscribe and publish to<br />
- les paramètres Kp, Ki, Kd pour les deux boucles PID au cas où vos pompes péristaltiques auraient un débit différent de 1,5 litre/heure pour la pompe pH et de 3,0 litres/heure pour la pompe chlore. De plus, les valeurs Kp par défaut ont été ajustées pour un volume de piscine de 50 m3. Vous devrez peut-être ajuster les valeurs de Kp en cas de volume de piscine et/ou de débit de pompes péristaltiques différents (commencez par l'ajuster proportionnellement). Dans tous les cas ces paramètres sont susceptibles de nécessiter des ajustements pour chaque piscine<br /></p>

<h4>Astuces</h4>
Avant de tenter de réguler l'eau de votre piscine avec cet automate, il est indispensable de commencer par :<br />
1- Tester la qualité de votre eau (à l'aide de kits liquides et/ou de bandelettes de test par exemple) et l'équilibrer correctement (pH, Chlore, Alcalinité, Dureté). Un bon équilibrage de l'eau facilitera grandement la stabilité et la régulation du pH<br />
2- Calibrer la sonde de pH à l'aide de solutions tampons calibrées (attention à la température de l'eau qui joue un grand rôle dans les mesures de pH)<br />
3- Ajuster le pH à 7,4 <br />
4- Une fois que les étapes 1 à 3 ci-dessus sont correctes, vous pouvez commencer à réguler l'ORP<br /><br />
 
Notes:<br />
a/ le capteur ORP ne doit théoriquement pas être calibré ni compensé en température (par nature, son point de pivot 0mV ne peut pas se déplacer)<br />
b/ la lecture ORP est fortement affectée par la valeur du pH et la température de l'eau. Assurez-vous que le pH est ajusté à 7,4<br />
c/ préférez les sondes ORP en platine pour les points de consigne> 500 mV (c'est-à-dire les piscines et les spas)<br />
e/ le temps de réponse des capteurs ORP peut être rapide dans les solutions tampons de référence (10 secondes) et pourtant très lent dans l'eau de la piscine (minutes ou plus) car il dépend de la composition de l'eau <br /><br />


<p align="center"> <img src="/docs/Coffret.jpeg" width="400" title="Overview"> </p> <br /><br />
<p align="center"> <img src="/docs/Coffret1.jpeg" width="400" title="Overview"> </p> <br /><br />
<p align="center"> <img src="/docs/CarteInterface.jpeg" width="400" title="Overview"> </p> <br /><br />
<p align="center"> <img src="/docs/pcb.png" width="300" title="Overview"> </p> <br /><br />

<h4>MQTT API</h4>
<p>
Ci-dessous les Payloads/commandes à publier sur le topic "PoolTopicAPI" (voir codés en dur dans le code) au format Json afin de lancer des actions sur l'Arduino :<br />
<ul>
<li>{"Mode":1} or {"Mode":0}         -> set "Mode" to manual (0) or Auto (1). In Auto, filtration starts/stops at set times of the day and pH and Orp are regulated</li> 
<li>{"Heat":1} or {"Heat":0}         -> start/stop the regulation of the pool water temperature</li>
<li>{"FiltPump":1} or {"FiltPump":0} -> manually start/stop the filtration pump</li>
<li>{"ChlPump":1} or {"ChlPump":0}   -> manually start/stop the Chl pump to add more Chlorine</li>
<li>{"PhPump":1} or {"PhPump":0}     -> manually start/stop the Acid pump to lower the Ph</li>
<li>{"PhPID":1} or {"PhPID":0}       -> start/stop the Ph PID regulation loop</li>
<li>{"OrpPID":1} or {"OrpPID":0}     -> start/stop the Orp PID regulation loop</li>
<li>{"PhCalib":[4.02,3.8,9.0,9.11]}  -> multi-point linear regression calibration (minimum 1 point-couple, 6 max.) in the form [ProbeReading_0, BufferRating_0, xx, xx, ProbeReading_n, BufferRating_n]
<li>{"OrpCalib":[450,465,750,784]}   -> multi-point linear regression calibration (minimum 1 point-couple, 6 max.) in the form [ProbeReading_0, BufferRating_0, xx, xx, ProbeReading_n, BufferRating_n]
<li>{"PhSetPoint":7.4}               -> set the Ph setpoint, 7.4 in this example</li>
<li>{"OrpSetPoint":750.0}            -> set the Orp setpoint, 750mV in this example</li>
<li>{"WSetPoint":27.0}               -> set the water temperature setpoint, 27.0deg in this example</li>
<li>{"WTempLow":10.0}                -> set the water low-temperature threshold below which there is no need to regulate Orp and Ph (ie. in winter)</li>
<li>{"OrpPIDParams":[2857,0,0]}      -> respectively set Kp,Ki,Kd parameters of the Orp PID loop. In this example they are set to 2857, 0 and 0</li>
<li>{"PhPIDParams":[1330000,0,0.0]}  -> respectively set Kp,Ki,Kd parameters of the Ph PID loop. In this example they are set to 1330000, 0 and 0</li>
<li>{"OrpPIDWSize":3600000}           -> set the window size of the Orp PID loop in msec, 60mins in this example</li>
<li>{"PhPIDWSize":1200000}            -> set the window size of the Ph PID loop in msec, 20mins in this example</li>
<li>{"Date":[1,1,1,18,13,32,0]}      -> set date/time of RTC module in the following format: (Day of the month, Day of the week, Month, Year, Hour, Minute, Seconds), in this example: Monday 1st January 2018 - 13h32mn00secs</li>
<li>{"FiltT0":9}                     -> set the earliest hour (9:00 in this example) to run filtration pump. Filtration pump will not run beofre that hour</li>
<li>{"FiltT1":20}                    -> set the latest hour (20:00 in this example) to run filtration pump. Filtration pump will not run after that hour</li>
<li>{"PubPeriod":30}                 -> set the periodicity (in seconds) at which the system info (pumps states, tank levels states, measured values, etc) will be published to the MQTT broker</li>
<li>{"PumpsMaxUp":1800}              -> set the Max Uptime (in secs) for the Ph and Chl pumps over a 24h period. If over, PID regulation is stopped and a warning flag is raised</li>
<li>{"Clear":1}                      -> reset the pH and Orp pumps overtime error flags in order to let the regulation loops continue. "Mode" also needs to be switched back to Auto (1) after an error flag was raised</li>
<li>{"DelayPID":30}                  -> Delay (in mins) after FiltT0 before the PID regulation loops will start. This is to let the Orp and pH readings stabilize first. 30mins in this example. Should not be > 59mins</li>
<li>{"TempExt":4.2}                  -> Provide the system with the external temperature. Should be updated regularly and will be used to start filtration when temperature is less than 2°C. 4.2deg in this example</li>
<li>{"PSIHigh":1.0}                  -> set the water high-pressure threshold (1.0bar in this example). When water pressure is over that threshold, an error flag is set</li>
<li>{"pHTank":[20,100]}              -> call this function when the Acid tank is replaced or refilled. First parameter is the tank volume in Liters, second parameter is its percentage fill (100% when full)</li>
<li>{"ChlTank":[20,100]}             -> call this function when the Chlorine tank is replaced or refilled. First parameter is the tank volume in Liters, second parameter is its percentage fill (100% when full)</li>
<li>{"Relay":[1,1]}                  -> call this generic command to actuate spare relays. Parameter 1 is the relay number (R1 in this example), parameter 2 is the relay state (ON in this example). This command is useful to use spare relays for additional features (lighting, etc). Available relay numbers are 1,2,6,7,8,9</li>
<li>{"Reboot":1}                     -> call this command to reboot the controller (after 8 seconds from calling this command)</li>
<li>{"pHPumpFR":1.5}                 -> call this command to set pH pump flow rate un L/s. In this example 1.5L/s</li>
<li>{"ChlPumpFR":3}                  -> call this command to set Chl pump flow rate un L/s. In this example 3L/s</li>
<li>{"RstpHCal":1}                   -> call this command to reset the calibration coefficients of the pH probe</li>
<li>{"RstOrpCal":1}                  -> call this command to reset the calibration coefficients of the Orp probe</li>
<li>{"RstPSICal":1}                  -> call this command to reset the calibration coefficients of the pressure sensor</li>


</ul>
</p><br />


<h4>Materiels</h4>
<p>
<ul>
<li><a title="https://fr.aliexpress.com/item/32864836449.html?spm=a2g0o.productlist.0.0.7e437288PyW6za&algo_pvid=439b0fe5-7cdc-46be-ab3f-49564465d3bf&algo_exp_id=439b0fe5-7cdc-46be-ab3f-49564465d3bf-0&pdp_ext_f=%7B%22sku_id%22%3A%2265815323341%22%7D&pdp_npi=2%40dis%21EUR%2113.54%2110.83%21%21%212.45%21%21%402100bdec16617690698694032e4c08%2165815323341%21sea&curPageLogUid=FT4ZhX6FH2za" href="https://fr.aliexpress.com/item/32864836449.html?spm=a2g0o.productlist.0.0.7e437288PyW6za&algo_pvid=439b0fe5-7cdc-46be-ab3f-49564465d3bf&algo_exp_id=439b0fe5-7cdc-46be-ab3f-49564465d3bf-0&pdp_ext_f=%7B%22sku_id%22%3A%2265815323341%22%7D&pdp_npi=2%40dis%21EUR%2113.54%2110.83%21%21%212.45%21%21%402100bdec16617690698694032e4c08%2165815323341%21sea&curPageLogUid=FT4ZhX6FH2za">x1 Arduino Mega 2560 + Ethernet shield + relay shield + RTC module</li>
<li><a title="https://www.phidgets.com/?tier=3&catid=11&pcid=9&prodid=103" href="https://www.phidgets.com/?tier=3&catid=11&pcid=9&prodid=103">x2 Phidgets PH/ORB amplifier modules</a></li> 
<li><a title="https://www.dfrobot.com/product-1621.html" href="https://www.dfrobot.com/product-1621.html">x2 Galvanic isolator for the pH and Orp probes</a></li> 
<li><a title="https://fr.aliexpress.com/item/32890406674.html?spm=a2g0o.order_list.0.0.31e55e5b4IVcHs&gatewayAdapt=glo2fra" href="https://fr.aliexpress.com/item/32890406674.html?spm=a2g0o.order_list.0.0.31e55e5b4IVcHs&gatewayAdapt=glo2fra">x2 pompes péristaltique de précision, pH et Orp</a></li>
<li><a title="http://electrolyseur.fr/pool-terre.html" href="http://electrolyseur.fr/pool-terre.html">x1 Water grounding</a></li>
<li><a title="http://electrolyseur.fr/kit-sonde-DS18B20-filtration-piscine.html" href="http://electrolyseur.fr/kit-sonde-DS18B20-filtration-piscine.html">x3 Sondes de temperature (DS18B20)</a></li>
<li><a title="https://fr.aliexpress.com/item/OOTDTY-G1-4-Pouces-5-v-0-0-5-MPa-Pression-Capteur-Capteur-D-huile-Carburant/32851667666.html?transAbTest=ae803_5&ws_ab_test=searchweb0_0%2Csearchweb201602_3_10065_10068_319_10892_317_10696_10084_453_454_10083_10618_10304_10307_10820_10821_537_10302_536_10902_10843_10059_10884_10887_321_322_10103%2Csearchweb201603_57%2CppcSwitch_0&algo_pvid=2456b33d-d7ee-4515-863d-af0c6b322395&algo_expid=2456b33d-d7ee-4515-863d-af0c6b322395-20" href="https://fr.aliexpress.com/item/OOTDTY-G1-4-Pouces-5-v-0-0-5-MPa-Pression-Capteur-Capteur-D-huile-Carburant/32851667666.html?transAbTest=ae803_5&ws_ab_test=searchweb0_0%2Csearchweb201602_3_10065_10068_319_10892_317_10696_10084_453_454_10083_10618_10304_10307_10820_10821_537_10302_536_10902_10843_10059_10884_10887_321_322_10103%2Csearchweb201603_57%2CppcSwitch_0&algo_pvid=2456b33d-d7ee-4515-863d-af0c6b322395&algo_expid=2456b33d-d7ee-4515-863d-af0c6b322395-20">x1 Capteur de Pression</a></li>
<li><a title="https://www.itead.cc/nextion-nx4832k035.html" href="https://www.itead.cc/nextion-nx4832k035.html">x1 Nextion Enhanced NX4832K035 - Generic 3.5'' HMI Touch Display</a></li>
<li><a title="https://fr.aliexpress.com/item/32715724000.html?spm=a2g0o.cart.0.0.202b378dusFRiW&mp=1&gatewayAdapt=glo2fra" href="https://fr.aliexpress.com/item/32715724000.html?spm=a2g0o.cart.0.0.202b378dusFRiW&mp=1&gatewayAdapt=glo2fra">x2 Capteur de Puissance
<li><a title="https://fr.aliexpress.com/item/1005002176327047.html?spm=a2g0o.productlist.0.0.4fe0387cFCRlgk&algo_pvid=68d7faf7-f8a2-4ab9-9c62-e5154ad8a35b&algo_exp_id=68d7faf7-f8a2-4ab9-9c62-e5154ad8a35b-0&pdp_ext_f=%7B%22sku_id%22%3A%2212000018942928718%22%7D&pdp_npi=2%40dis%21EUR%212.29%211.71%21%21%212.74%21%21%402100bde316617694618363670ef938%2112000018942928718%21sea&curPageLogUid=Za8JRt8PFbMH" href="https://fr.aliexpress.com/item/1005002176327047.html?spm=a2g0o.productlist.0.0.4fe0387cFCRlgk&algo_pvid=68d7faf7-f8a2-4ab9-9c62-e5154ad8a35b&algo_exp_id=68d7faf7-f8a2-4ab9-9c62-e5154ad8a35b-0&pdp_ext_f=%7B%22sku_id%22%3A%2212000018942928718%22%7D&pdp_npi=2%40dis%21EUR%212.29%211.71%21%21%212.74%21%21%402100bde316617694618363670ef938%2112000018942928718%21sea&curPageLogUid=Za8JRt8PFbMH">x2 Cellules de charge
<li><a title="https://fr.aliexpress.com/item/32566456466.html?spm=a2g0o.productlist.0.0.4db43822DJJd0l&algo_pvid=9683f5d0-5476-41df-8af5-6ed0fa830355&algo_exp_id=9683f5d0-5476-41df-8af5-6ed0fa830355-0&pdp_ext_f=%7B%22sku_id%22%3A%2258373183866%22%7D&pdp_npi=2%40dis%21EUR%210.47%210.38%21%21%21%21%21%402100bddb16617698476563893e6310%2158373183866%21sea&curPageLogUid=VYbIYbAi0ne9" href="https://fr.aliexpress.com/item/32566456466.html?spm=a2g0o.productlist.0.0.4db43822DJJd0l&algo_pvid=9683f5d0-5476-41df-8af5-6ed0fa830355&algo_exp_id=9683f5d0-5476-41df-8af5-6ed0fa830355-0&pdp_ext_f=%7B%22sku_id%22%3A%2258373183866%22%7D&pdp_npi=2%40dis%21EUR%210.47%210.38%21%21%21%21%21%402100bddb16617698476563893e6310%2158373183866%21sea&curPageLogUid=VYbIYbAi0ne9">x1 Carte de Module de convertisseur MAX485 TTL
<li><a title="http://" href="http://">x1 PCB by Nico
	
</ul>
</p><br />


<h4>Exemple d'integration (<a title="https://blynk.io/" href="https://blynk.io/">BLYNK</a>)</h4>
<p>
See NodeRed folder for more info and code
<p align="center"> <img src="/docs/BlynkPool.jpg" width="302" title="Blynk"> </p> <br />
