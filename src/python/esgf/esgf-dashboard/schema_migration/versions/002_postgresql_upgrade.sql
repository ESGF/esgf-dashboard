--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
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
    idrssfeed serial primary key, 
    rssfeed character varying(1024) NOT NULL,
    local integer DEFAULT 0,
    daterssfeed timestamp without time zone DEFAULT now() NOT NULL
);


ALTER TABLE esgf_dashboard.rssfeed OWNER TO dbsuper;


--
-- Name: rssfeed_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

--ALTER TABLE ONLY rssfeed
--    ADD CONSTRAINT rssfeed_pkey PRIMARY KEY (idrssfeed);


--
-- PostgreSQL database dump complete
--

--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
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
    idhost bigint NOT NULL
);


ALTER TABLE esgf_dashboard.hasfeed OWNER TO dbsuper;

--
-- Name: hasfeed_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace: 
--

--
-- Name: hasfeed_idhost_fkey; Type: FK CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper
--

ALTER TABLE ONLY hasfeed 
    ADD CONSTRAINT hasfeed_idhost_fkey FOREIGN KEY (idhost) REFERENCES host(id);

ALTER TABLE ONLY hasfeed 
    ADD CONSTRAINT hasfeed_idrssfeed_fkey FOREIGN KEY (idrssfeed) REFERENCES rssfeed(idrssfeed);
