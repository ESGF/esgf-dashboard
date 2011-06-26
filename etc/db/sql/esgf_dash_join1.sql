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
-- Name: join1; Type: TABLE; Schema: public; Owner: esgcet; Tablespace: 
--

CREATE TABLE join1 (
    iduser bigint NOT NULL,
    idproject bigint NOT NULL
);


ALTER TABLE public.join1 OWNER TO esgcet;

--
-- Data for Name: join1; Type: TABLE DATA; Schema: public; Owner: esgcet
--

COPY join1 (iduser, idproject) FROM stdin;
1	9
4	9
4	6
\.


--
-- Name: join1_pkey; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY join1
    ADD CONSTRAINT join1_pkey PRIMARY KEY (iduser, idproject);


--
-- Name: join1_iduser_fkey; Type: FK CONSTRAINT; Schema: public; Owner: esgcet
--

ALTER TABLE ONLY join1
    ADD CONSTRAINT join1_iduser_fkey FOREIGN KEY (iduser) REFERENCES user1(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

