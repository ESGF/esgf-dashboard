--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: country; Type: TABLE; Schema: public; Owner: esgcet; Tablespace: 
--

CREATE TABLE country (
    id integer NOT NULL,
    name character varying(64) NOT NULL
);


ALTER TABLE public.country OWNER TO esgcet;

--
-- Name: country_id_seq; Type: SEQUENCE; Schema: public; Owner: esgcet
--

CREATE SEQUENCE country_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.country_id_seq OWNER TO esgcet;

--
-- Name: country_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: esgcet
--

ALTER SEQUENCE country_id_seq OWNED BY country.id;


--
-- Name: country_id_seq; Type: SEQUENCE SET; Schema: public; Owner: esgcet
--

SELECT pg_catalog.setval('country_id_seq', 1, false);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: esgcet
--

ALTER TABLE country ALTER COLUMN id SET DEFAULT nextval('country_id_seq'::regclass);


--
-- Data for Name: country; Type: TABLE DATA; Schema: public; Owner: esgcet
--

COPY country (id, name) FROM stdin;
245	Afghanistan, Islamic State of
246	Albania
247	Algeria
248	American Samoa
249	Andorra, Principality of
250	Angola
251	Anguilla
252	Antarctica
253	Antigua and Barbuda
254	Argentina
255	Armenia
256	Aruba
257	Australia
258	Austria
259	Azerbaidjan
260	Bahamas
261	Bahrain
262	Bangladesh
263	Barbados
264	Belarus
265	Belgium
266	Belize
267	Benin
268	Bermuda
269	Bhutan
270	Bolivia
271	Bosnia-Herzegovina
272	Botswana
273	Bouvet Island
274	Brazil
275	British Indian Ocean Territory
276	Brunei Darussalam
277	Bulgaria
278	Burkina Faso
279	Burundi
280	Cambodia, Kingdom of
281	Cameroon
282	Canada
283	Cape Verde
284	Cayman Islands
285	Central African Republic
286	Chad
287	Chile
288	China
289	Christmas Island
290	Cocos (Keeling) Islands
291	Colombia
292	Comoros
293	Congo
294	Congo, The Democratic Republic of the
295	Cook Islands
296	Costa Rica
297	Croatia
298	Cuba
299	Cyprus
300	Czech Republic
301	Denmark
302	Djibouti
303	Dominica
304	Dominican Republic
305	East Timor
306	Ecuador
307	Egypt
308	El Salvador
309	Equatorial Guinea
310	Eritrea
311	Estonia
312	Ethiopia
313	Falkland Islands
314	Faroe Islands
315	Fiji
316	Finland
317	Former Czechoslovakia
318	Former USSR
319	France
320	France (European Territory)
321	French Guyana
322	French Southern Territories
323	Gabon
324	Gambia
325	Georgia
326	Germany
327	Ghana
328	Gibraltar
329	Great Britain
330	Greece
331	Greenland
332	Grenada
333	Guadeloupe (French)
334	Guam (USA)
335	Guatemala
336	Guinea
337	Guinea Bissau
338	Guyana
339	Haiti
340	Heard and McDonald Islands
341	Holy See (Vatican City State)
342	Honduras
343	Hong Kong
344	Hungary
345	Iceland
346	India
347	Indonesia
348	Iran
349	Iraq
350	Ireland
351	Israel
352	Italy
353	Ivory Coast
354	Jamaica
355	Japan
356	Jordan
357	Kazakhstan
358	Kenya
359	Kiribati
360	Kuwait
361	Kyrgyz Republic (Kyrgyzstan)
362	Laos
363	Latvia
364	Lebanon
365	Lesotho
366	Liberia
367	Libya
368	Liechtenstein
369	Lithuania
370	Luxembourg
371	Macau
372	Macedonia
373	Madagascar
374	Malawi
375	Malaysia
376	Maldives
377	Mali
378	Malta
379	Marshall Islands
380	Martinique (French)
381	Mauritania
382	Mauritius
383	Mayotte
384	Mexico
385	Micronesia
386	Moldavia
387	Monaco
388	Mongolia
389	Montserrat
390	Morocco
391	Mozambique
392	Myanmar
393	Namibia
394	Nauru
395	Nepal
396	Netherlands
397	Netherlands Antilles
398	Neutral Zone
399	New Caledonia (French)
400	New Zealand
401	Nicaragua
402	Niger
403	Nigeria
404	Niue
405	Norfolk Island
406	North Korea
407	Northern Mariana Islands
408	Norway
409	Oman
410	Pakistan
411	Palau
412	Panama
413	Papua New Guinea
414	Paraguay
415	Peru
416	Philippines
417	Pitcairn Island
418	Poland
419	Polynesia (French)
420	Portugal
421	Puerto Rico
422	Qatar
423	Reunion (French)
424	Romania
425	Russian Federation
426	Rwanda
427	S. Georgia & S. Sandwich Isls.
428	Saint Helena
429	Saint Kitts & Nevis Anguilla
430	Saint Lucia
431	Saint Pierre and Miquelon
432	Saint Tome (Sao Tome) and Principe
433	Saint Vincent & Grenadines
434	Samoa
435	San Marino
436	Saudi Arabia
437	Senegal
438	Seychelles
439	Sierra Leone
440	Singapore
441	Slovak Republic
442	Slovenia
443	Solomon Islands
444	Somalia
445	South Africa
446	South Korea
447	Spain
448	Sri Lanka
449	Sudan
450	Suriname
451	Svalbard and Jan Mayen Islands
452	Swaziland
453	Sweden
454	Switzerland
455	Syria
456	Tadjikistan
457	Taiwan
458	Tanzania
459	Thailand
460	Togo
461	Tokelau
462	Tonga
463	Trinidad and Tobago
464	Tunisia
465	Turkey
466	Turkmenistan
467	Turks and Caicos Islands
468	Tuvalu
469	Uganda
470	Ukraine
471	United Arab Emirates
472	United Kingdom
473	United States
474	Uruguay
475	USA Minor Outlying Islands
476	Uzbekistan
477	Vanuatu
478	Venezuela
479	Vietnam
480	Virgin Islands (British)
481	Virgin Islands (USA)
482	Wallis and Futuna Islands
483	Western Sahara
484	Yemen
485	Yugoslavia
486	Zaire
487	Zambia
488	Zimbabwe
\.


--
-- Name: country_pkey; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY country
    ADD CONSTRAINT country_pkey PRIMARY KEY (id);


--
-- PostgreSQL database dump complete
--

