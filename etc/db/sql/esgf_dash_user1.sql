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
-- Name: user1; Type: TABLE; Schema: public; Owner: esgcet; Tablespace: 
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


ALTER TABLE public.user1 OWNER TO esgcet;

--
-- Name: user1_id_seq; Type: SEQUENCE; Schema: public; Owner: esgcet
--

CREATE SEQUENCE user1_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE public.user1_id_seq OWNER TO esgcet;

--
-- Name: user1_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: esgcet
--

ALTER SEQUENCE user1_id_seq OWNED BY user1.id;


--
-- Name: user1_id_seq; Type: SEQUENCE SET; Schema: public; Owner: esgcet
--

SELECT pg_catalog.setval('user1_id_seq', 3, true);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: esgcet
--

ALTER TABLE user1 ALTER COLUMN id SET DEFAULT nextval('user1_id_seq'::regclass);


--
-- Data for Name: user1; Type: TABLE DATA; Schema: public; Owner: esgcet
--

COPY user1 (dn, id, name, surname, mail, username, password, registrationdate, accountcertified, idcountry) FROM stdin;
\N	1	guest	guest	guest@guest.com	guest	d89d9a8b05e6e70040ca5b0e00db6c43	2010-09-28 14:56:06	1	\N
\N	4	admin	admin	admin@admin.com	dashgadmin	d605b31a9457a07f2313b435ab5b7708	2010-09-28 17:39:56	1	352
\.


--
-- Name: user1_dn_key; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY user1
    ADD CONSTRAINT user1_dn_key UNIQUE (dn);


--
-- Name: user1_pkey; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY user1
    ADD CONSTRAINT user1_pkey PRIMARY KEY (id);


--
-- Name: user1_username_key; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY user1
    ADD CONSTRAINT user1_username_key UNIQUE (username);


--
-- Name: user1_idcountry_fkey; Type: FK CONSTRAINT; Schema: public; Owner: esgcet
--

ALTER TABLE ONLY user1
    ADD CONSTRAINT user1_idcountry_fkey FOREIGN KEY (idcountry) REFERENCES country(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

