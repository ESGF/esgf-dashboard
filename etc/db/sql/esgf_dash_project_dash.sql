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
-- Name: project_dash; Type: TABLE; Schema: public; Owner: esgcet; Tablespace: 
--

CREATE TABLE project_dash (
    id integer NOT NULL,
    name character varying(255) NOT NULL,
    description character varying(2000),
    startdate timestamp without time zone DEFAULT now() NOT NULL,
    enddate timestamp without time zone,
    regpublic integer DEFAULT 0 NOT NULL
);


ALTER TABLE public.project_dash OWNER TO esgcet;

--
-- Name: project_dash_id_se; Type: SEQUENCE; Schema: public; Owner: esgcet
--

CREATE SEQUENCE project_dash_id_se
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.project_dash_id_se OWNER TO esgcet;

--
-- Name: project_dash_id_se; Type: SEQUENCE OWNED BY; Schema: public; Owner: esgcet
--

ALTER SEQUENCE project_dash_id_se OWNED BY project_dash.id;


--
-- Name: project_dash_id_se; Type: SEQUENCE SET; Schema: public; Owner: esgcet
--

SELECT pg_catalog.setval('project_dash_id_se', 1, false);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: esgcet
--

ALTER TABLE project_dash ALTER COLUMN id SET DEFAULT nextval('project_dash_id_se'::regclass);


--
-- Data for Name: project_dash; Type: TABLE DATA; Schema: public; Owner: esgcet
--

COPY project_dash (id, name, description, startdate, enddate, regpublic) FROM stdin;
6	ESG Federation	The Earth System Grid Federation (ESGF) is a spontaneous, unfunded collaboration of people and institutions working together to build an open source software infrastructure for the management and analysis of Earth Science data	2010-09-28 14:43:21	\N	0
9	Local DataNode	Local Data Node Monitoring	2011-02-24 11:00:26	\N	0
\.


--
-- PostgreSQL database dump complete
--

