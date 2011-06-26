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
-- Name: service_instance; Type: TABLE; Schema: public; Owner: esgcet; Tablespace: 
--

CREATE TABLE service_instance (
    id integer NOT NULL,
    port bigint NOT NULL,
    name character varying(255),
    institution character varying(255),
    mail_admin character varying(255),
    idhost bigint NOT NULL
);


ALTER TABLE public.service_instance OWNER TO esgcet;

--
-- Name: service_instance_id_seq; Type: SEQUENCE; Schema: public; Owner: esgcet
--

CREATE SEQUENCE service_instance_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.service_instance_id_seq OWNER TO esgcet;

--
-- Name: service_instance_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: esgcet
--

ALTER SEQUENCE service_instance_id_seq OWNED BY service_instance.id;


--
-- Name: service_instance_id_seq; Type: SEQUENCE SET; Schema: public; Owner: esgcet
--

SELECT pg_catalog.setval('service_instance_id_seq', 18, true);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: esgcet
--

ALTER TABLE service_instance ALTER COLUMN id SET DEFAULT nextval('service_instance_id_seq'::regclass);


--
-- Data for Name: service_instance; Type: TABLE DATA; Schema: public; Owner: esgcet
--

COPY service_instance (id, port, name, institution, mail_admin, idhost) FROM stdin;
29	80	OPeNDAP	\N	\N	17
31	80	OPeNDAP	\N	\N	19
1	5432	Postgresql	\N	\N	30
30	80	OPeNDAP	\N	\N	18
8	80	Tomcat	\N	\N	2
9	80	OPeNDAP	\N	\N	3
12	80	OPeNDAP	\N	\N	4
13	80	OPeNDAP	\N	\N	5
2	2811	GriFTP_download	\N	\N	17
3	2811	GriFTP_download	\N	\N	18
4	2811	GriFTP_download	\N	\N	19
10	2811	GriFTP_download	\N	\N	3
14	2811	GriFTP_download	\N	\N	4
15	2811	GriFTP_download	\N	\N	5
5	2811	GriFTP_download	\N	\N	1
18	2811	GriFTP_download	\N	\N	18
6	2812	GriFTP_replication	\N	\N	1
7	2812	GriFTP_replication	\N	\N	2
11	2812	GriFTP_replication	\N	\N	3
16	2812	GriFTP_replication	\N	\N	5
17	2812	GriFTP_replication	\N	\N	4
\.


--
-- Name: service_instance_pkey; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY service_instance
    ADD CONSTRAINT service_instance_pkey PRIMARY KEY (id);


--
-- Name: service_instance_idhost_fkey; Type: FK CONSTRAINT; Schema: public; Owner: esgcet
--

ALTER TABLE ONLY service_instance
    ADD CONSTRAINT service_instance_idhost_fkey FOREIGN KEY (idhost) REFERENCES host(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

