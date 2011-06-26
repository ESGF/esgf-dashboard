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
-- Name: host; Type: TABLE; Schema: public; Owner: esgcet; Tablespace: 
--

CREATE TABLE host (
    id integer NOT NULL,
    ip character varying(64) NOT NULL,
    name character varying(255),
    city character varying(255),
    latitude numeric(9,6),
    longitude numeric(9,6)
);


ALTER TABLE public.host OWNER TO esgcet;

--
-- Name: host_id_seq; Type: SEQUENCE; Schema: public; Owner: esgcet
--

CREATE SEQUENCE host_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.host_id_seq OWNER TO esgcet;

--
-- Name: host_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: esgcet
--

ALTER SEQUENCE host_id_seq OWNED BY host.id;


--
-- Name: host_id_seq; Type: SEQUENCE SET; Schema: public; Owner: esgcet
--

SELECT pg_catalog.setval('host_id_seq', 5, true);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: esgcet
--

ALTER TABLE host ALTER COLUMN id SET DEFAULT nextval('host_id_seq'::regclass);


--
-- Data for Name: host; Type: TABLE DATA; Schema: public; Owner: esgcet
--

COPY host (id, ip, name, city, latitude, longitude) FROM stdin;
17	esg-dev1.badc.rl.ac.uk	BADC esg-dev1	London	51.500152	-0.126236
2	esg-datanode.jpl.nasa.gov	JPL_datanode	Pasadena	34.135929	-118.125400
3	esg-vm-demo03.ccs.ornl.gov	ORNL_demo03	Oak Ridge	35.932029	-84.310765
5	esgf-node3.llnl.gov	LLNL_node3	Livermore	37.688185	-121.710996
4	esgf-node1.llnl.gov	LLNL_node1	Livermore	37.000000	-121.000000
18	cmip1.dkrz.de	DKRZ cmip1	Hamburg	53.553813	9.991586
19	adm02.cmcc.it	CMCC adm02	Lecce	40.353191	18.173915
1	cmip2.dkrz.de	DKRZ cmip2	Hamburg	53.000000	9.500000
30	127.0.0.1	localhost	City	40.350000	18.170000
\.


--
-- Name: host_ip_key; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY host
    ADD CONSTRAINT host_ip_key UNIQUE (ip);


--
-- Name: host_pkey; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY host
    ADD CONSTRAINT host_pkey PRIMARY KEY (id);


--
-- PostgreSQL database dump complete
--

