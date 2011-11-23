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
-- Name: rssfeed; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE rssfeed (
    idrssfeed integer NOT NULL,
    rssfeed character varying(1024) NOT NULL,
    local integer DEFAULT 0,
    daterssfeed timestamp without time zone DEFAULT now() NOT NULL
);


ALTER TABLE esgf_dashboard.rssfeed OWNER TO dbsuper;

--
-- Name: rssfeed_idrssfeed_seq; Type: SEQUENCE; Schema: esgf_dashboard; Owner: dbsuper
--

CREATE SEQUENCE rssfeed_idrssfeed_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;


ALTER TABLE esgf_dashboard.rssfeed_idrssfeed_seq OWNER TO dbsuper;

--
-- Name: rssfeed_idrssfeed_seq; Type: SEQUENCE OWNED BY; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER SEQUENCE rssfeed_idrssfeed_seq OWNED BY rssfeed.idrssfeed;


--
-- Name: rssfeed_idrssfeed_seq; Type: SEQUENCE SET; Schema: esgf_dashboard; Owner: dbsuper
--

SELECT pg_catalog.setval('rssfeed_idrssfeed_seq', 1, true);


--
-- Name: idrssfeed; Type: DEFAULT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE rssfeed ALTER COLUMN idrssfeed SET DEFAULT nextval('rssfeed_idrssfeed_seq'::regclass);


--
-- Name: rssfeed_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY rssfeed
    ADD CONSTRAINT rssfeed_pkey PRIMARY KEY (idrssfeed);


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
-- Name: hasfeed; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

CREATE TABLE hasfeed (
    idrssfeed bigint NOT NULL,
    idhost bigint NOT NULL,
    startdate timestamp without time zone DEFAULT now() NOT NULL,
    enddate timestamp without time zone
);


ALTER TABLE esgf_dashboard.hasfeed OWNER TO dbsuper;

--
-- Name: hasfeed_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

ALTER TABLE ONLY hasfeed 
    ADD CONSTRAINT hasfeed_pkey PRIMARY KEY (idrssfeed, idhost, startdate);

--
-- Name: hasfeed_idhost_fkey; Type: FK CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE ONLY hasfeed 
    ADD CONSTRAINT hasfeed_idhost_fkey FOREIGN KEY (idhost) REFERENCES host(id);

ALTER TABLE ONLY hasfeed 
    ADD CONSTRAINT hasfeed_idrssfeed_fkey FOREIGN KEY (idrssfeed) REFERENCES rssfeed(idrssfeed);
