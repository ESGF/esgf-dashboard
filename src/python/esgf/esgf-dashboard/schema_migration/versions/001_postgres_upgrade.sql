CREATE SCHEMA esgf_dashboard;
--
-- PostgreSQL database dump
--

-- SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: country; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE country (
    id integer NOT NULL,
    name character varying(64) NOT NULL
);


ALTER TABLE esgf_dashboard.country OWNER TO dbsuper;

--
-- Name: country_id_seq; Type: SEQUENCE; Schema: esgf_dashboard; Owner: dbsuper
--

CREATE SEQUENCE country_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE esgf_dashboard.country_id_seq OWNER TO dbsuper;

--
-- Name: country_id_seq; Type: SEQUENCE OWNED BY; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER SEQUENCE country_id_seq OWNED BY country.id;


--
-- Name: country_id_seq; Type: SEQUENCE SET; Schema: esgf_dashboard; Owner: dbsuper
--

SELECT pg_catalog.setval('country_id_seq', 1, false);


--
-- Name: id; Type: DEFAULT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE country ALTER COLUMN id SET DEFAULT nextval('country_id_seq'::regclass);


--
-- Data for Name: country; Type: TABLE DATA; Schema: esgf_dashboard; Owner: dbsuper
--


--
-- Name: country_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY country
    ADD CONSTRAINT country_pkey PRIMARY KEY (id);


--
-- PostgreSQL database dump complete
--

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: news; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE news (
    idnews integer NOT NULL,
    news character varying(255) NOT NULL,
    datenews timestamp without time zone DEFAULT now() NOT NULL
);


ALTER TABLE esgf_dashboard.news OWNER TO dbsuper;

--
-- Name: news_idnews_seq; Type: SEQUENCE; Schema: esgf_dashboard; Owner: dbsuper
--

CREATE SEQUENCE news_idnews_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE esgf_dashboard.news_idnews_seq OWNER TO dbsuper;

--
-- Name: news_idnews_seq; Type: SEQUENCE OWNED BY; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER SEQUENCE news_idnews_seq OWNED BY news.idnews;


--
-- Name: news_idnews_seq; Type: SEQUENCE SET; Schema: esgf_dashboard; Owner: dbsuper
--

SELECT pg_catalog.setval('news_idnews_seq', 1, true);


--
-- Name: idnews; Type: DEFAULT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE news ALTER COLUMN idnews SET DEFAULT nextval('news_idnews_seq'::regclass);


--
-- Data for Name: news; Type: TABLE DATA; Schema: esgf_dashboard; Owner: dbsuper
--

--
-- Name: news_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY news
    ADD CONSTRAINT news_pkey PRIMARY KEY (idnews);


--
-- PostgreSQL database dump complete
--

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: user1; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE user1 (
    dn character varying(255),
    id integer NOT NULL,
    name character varying(45) NOT NULL,
    surname character varying(45) NOT NULL,
    mail character varying(45) NOT NULL,
    username character varying(25) NOT NULL,
    password character varying(32) NOT NULL,
    registrationdate timestamp without time zone DEFAULT now() NOT NULL,
    accountcertified smallint DEFAULT 0 NOT NULL,
    idcountry integer
);


ALTER TABLE esgf_dashboard.user1 OWNER TO dbsuper;

--
-- Name: user1_id_seq; Type: SEQUENCE; Schema: esgf_dashboard; Owner: dbsuper
--

CREATE SEQUENCE user1_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE esgf_dashboard.user1_id_seq OWNER TO dbsuper;

--
-- Name: user1_id_seq; Type: SEQUENCE OWNED BY; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER SEQUENCE user1_id_seq OWNED BY user1.id;


--
-- Name: user1_id_seq; Type: SEQUENCE SET; Schema: esgf_dashboard; Owner: dbsuper
--

SELECT pg_catalog.setval('user1_id_seq', 1, true);


--
-- Name: id; Type: DEFAULT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE user1 ALTER COLUMN id SET DEFAULT nextval('user1_id_seq'::regclass);


--
-- Data for Name: user1; Type: TABLE DATA; Schema: esgf_dashboard; Owner: dbsuper
--

insert into user1(id,name,surname,mail,username,password,accountcertified) values(1,'guest','guest','guest@guest.gov','guest',MD5('password'),1);
--- COPY user1 (dn, id, name, surname, mail, username, password, registrationdate, accountcertified, idcountry) FROM stdin;
--- \N	1	guest	guest	guest@guest.com	guest	d89d9a8b05e6e70040ca5b0e00db6c43	2010-09-28 14:56:06	1	\N
--- \.


--
-- Name: user1_dn_key; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY user1
    ADD CONSTRAINT user1_dn_key UNIQUE (dn);


--
-- Name: user1_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY user1
    ADD CONSTRAINT user1_pkey PRIMARY KEY (id);


--
-- Name: user1_username_key; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY user1
    ADD CONSTRAINT user1_username_key UNIQUE (username);


--
-- Name: user1_idcountry_fkey; Type: FK CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE ONLY user1
    ADD CONSTRAINT user1_idcountry_fkey FOREIGN KEY (idcountry) REFERENCES country(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: host; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE host (
    id integer NOT NULL,
    ip character varying(64) NOT NULL,
    name character varying(255),
    city character varying(255),
    latitude numeric(9,6),
    longitude numeric(9,6)
);


ALTER TABLE esgf_dashboard.host OWNER TO dbsuper;

--
-- Name: host_id_seq; Type: SEQUENCE; Schema: esgf_dashboard; Owner: dbsuper
--

CREATE SEQUENCE host_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE esgf_dashboard.host_id_seq OWNER TO dbsuper;

--
-- Name: host_id_seq; Type: SEQUENCE OWNED BY; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER SEQUENCE host_id_seq OWNED BY host.id;


--
-- Name: host_id_seq; Type: SEQUENCE SET; Schema: esgf_dashboard; Owner: dbsuper
--

SELECT pg_catalog.setval('host_id_seq', 1, true);


--
-- Name: id; Type: DEFAULT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE host ALTER COLUMN id SET DEFAULT nextval('host_id_seq'::regclass);


--
-- Data for Name: host; Type: TABLE DATA; Schema: esgf_dashboard; Owner: dbsuper
--


--
-- Name: host_ip_key; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY host
    ADD CONSTRAINT host_ip_key UNIQUE (ip);


--
-- Name: host_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY host
    ADD CONSTRAINT host_pkey PRIMARY KEY (id);


--
-- PostgreSQL database dump complete
--

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: project_dash; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE project_dash (
    id integer NOT NULL,
    name character varying(255) NOT NULL,
    description character varying(2000),
    startdate timestamp without time zone DEFAULT now() NOT NULL,
    enddate timestamp without time zone,
    regpublic integer DEFAULT 0 NOT NULL
);


ALTER TABLE esgf_dashboard.project_dash OWNER TO dbsuper;

--
-- Name: project_dash_id_se; Type: SEQUENCE; Schema: esgf_dashboard; Owner: dbsuper
--

CREATE SEQUENCE project_dash_id_se
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE esgf_dashboard.project_dash_id_se OWNER TO dbsuper;

--
-- Name: project_dash_id_se; Type: SEQUENCE OWNED BY; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER SEQUENCE project_dash_id_se OWNED BY project_dash.id;


--
-- Name: project_dash_id_se; Type: SEQUENCE SET; Schema: esgf_dashboard; Owner: dbsuper
--

SELECT pg_catalog.setval('project_dash_id_se', 1, false);


--
-- Name: id; Type: DEFAULT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE project_dash ALTER COLUMN id SET DEFAULT nextval('project_dash_id_se'::regclass);


--
-- Data for Name: project_dash; Type: TABLE DATA; Schema: esgf_dashboard; Owner: dbsuper
--

--
-- PostgreSQL database dump complete
--

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: join1; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE join1 (
    iduser bigint NOT NULL,
    idproject bigint NOT NULL
);


ALTER TABLE esgf_dashboard.join1 OWNER TO dbsuper;

--
-- Data for Name: join1; Type: TABLE DATA; Schema: esgf_dashboard; Owner: dbsuper
--


--
-- Name: join1_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY join1
    ADD CONSTRAINT join1_pkey PRIMARY KEY (iduser, idproject);


--
-- Name: join1_iduser_fkey; Type: FK CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE ONLY join1
    ADD CONSTRAINT join1_iduser_fkey FOREIGN KEY (iduser) REFERENCES user1(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: service_instance; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE service_instance (
    id integer NOT NULL,
    port bigint NOT NULL,
    name character varying(255),
    institution character varying(255),
    mail_admin character varying(255),
    idhost bigint NOT NULL
);


ALTER TABLE esgf_dashboard.service_instance OWNER TO dbsuper;

--
-- Name: service_instance_id_seq; Type: SEQUENCE; Schema: esgf_dashboard; Owner: dbsuper
--

CREATE SEQUENCE service_instance_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE esgf_dashboard.service_instance_id_seq OWNER TO dbsuper;

--
-- Name: service_instance_id_seq; Type: SEQUENCE OWNED BY; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER SEQUENCE service_instance_id_seq OWNED BY service_instance.id;


--
-- Name: service_instance_id_seq; Type: SEQUENCE SET; Schema: esgf_dashboard; Owner: dbsuper
--

SELECT pg_catalog.setval('service_instance_id_seq', 1, true);


--
-- Name: id; Type: DEFAULT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE service_instance ALTER COLUMN id SET DEFAULT nextval('service_instance_id_seq'::regclass);


--
-- Data for Name: service_instance; Type: TABLE DATA; Schema: esgf_dashboard; Owner: dbsuper
--


--
-- Name: service_instance_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY service_instance
    ADD CONSTRAINT service_instance_pkey PRIMARY KEY (id);


--
-- Name: service_instance_idhost_fkey; Type: FK CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE ONLY service_instance
    ADD CONSTRAINT service_instance_idhost_fkey FOREIGN KEY (idhost) REFERENCES host(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: uses; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE uses (
    idproject bigint NOT NULL,
    idserviceinstance bigint NOT NULL,
    startdate timestamp without time zone DEFAULT now() NOT NULL,
    enddate timestamp without time zone
);


ALTER TABLE esgf_dashboard.uses OWNER TO dbsuper;

--
-- Data for Name: uses; Type: TABLE DATA; Schema: esgf_dashboard; Owner: dbsuper
--


--
-- Name: uses_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY uses
    ADD CONSTRAINT uses_pkey PRIMARY KEY (idproject, idserviceinstance, startdate);


--
-- Name: uses_idserviceinstance_fkey; Type: FK CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE ONLY uses
    ADD CONSTRAINT uses_idserviceinstance_fkey FOREIGN KEY (idserviceinstance) REFERENCES service_instance(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: service_status; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE service_status (
    id integer NOT NULL,
    "timestamp" timestamp without time zone DEFAULT now() NOT NULL,
    status integer NOT NULL,
    elapsedtime bigint NOT NULL,
    idserviceinstance bigint NOT NULL
);


ALTER TABLE esgf_dashboard.service_status OWNER TO dbsuper;

--
-- Name: service_status_id_seq; Type: SEQUENCE; Schema: esgf_dashboard; Owner: dbsuper
--

CREATE SEQUENCE service_status_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE esgf_dashboard.service_status_id_seq OWNER TO dbsuper;

--
-- Name: service_status_id_seq; Type: SEQUENCE OWNED BY; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER SEQUENCE service_status_id_seq OWNED BY service_status.id;


--
-- Name: service_status_id_seq; Type: SEQUENCE SET; Schema: esgf_dashboard; Owner: dbsuper
--

SELECT pg_catalog.setval('service_status_id_seq', 1, true);


--
-- Name: id; Type: DEFAULT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE service_status ALTER COLUMN id SET DEFAULT nextval('service_status_id_seq'::regclass);


--
-- Data for Name: service_status; Type: TABLE DATA; Schema: esgf_dashboard; Owner: dbsuper
--


--
-- Name: service_status_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY service_status
    ADD CONSTRAINT service_status_pkey PRIMARY KEY (id);


--
-- Name: service_status_index; Type: INDEX; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE INDEX service_status_index ON service_status USING btree (idserviceinstance);


--
-- Name: service_status_status; Type: INDEX; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE INDEX service_status_status ON service_status USING btree (status);


--
-- Name: service_status_timestamp; Type: INDEX; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE INDEX service_status_timestamp ON service_status USING btree ("timestamp");


--
-- Name: service_status_idserviceinstance_fkey; Type: FK CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE ONLY service_status
    ADD CONSTRAINT service_status_idserviceinstance_fkey FOREIGN KEY (idserviceinstance) REFERENCES service_instance(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

SET search_path = public, pg_catalog;
