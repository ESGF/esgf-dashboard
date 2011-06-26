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
-- Name: uses; Type: TABLE; Schema: public; Owner: esgcet; Tablespace: 
--

CREATE TABLE uses (
    idproject bigint NOT NULL,
    idserviceinstance bigint NOT NULL,
    startdate timestamp without time zone DEFAULT now() NOT NULL,
    enddate timestamp without time zone
);


ALTER TABLE public.uses OWNER TO esgcet;

--
-- Data for Name: uses; Type: TABLE DATA; Schema: public; Owner: esgcet
--

COPY uses (idproject, idserviceinstance, startdate, enddate) FROM stdin;
6	29	2010-09-28 17:38:31	\N
6	30	2010-09-28 17:38:31	\N
6	31	2010-09-28 17:38:31	\N
9	1	2011-04-29 19:27:49.976329	\N
6	5	2011-05-28 00:40:35.016614	\N
6	6	2011-05-28 00:40:37.639438	\N
6	7	2011-05-28 00:40:40.159685	\N
6	8	2011-05-28 00:40:42.061737	\N
6	9	2011-05-28 00:40:43.981678	\N
6	10	2011-05-28 00:40:47.519735	\N
6	11	2011-05-28 00:40:49.470025	\N
6	12	2011-05-28 00:40:51.31145	\N
6	13	2011-05-28 00:40:53.222125	\N
6	14	2011-05-28 00:40:56.615781	\N
6	15	2011-05-28 00:41:01.647612	\N
6	16	2011-05-28 00:41:03.533073	\N
6	17	2011-05-28 00:41:07.269887	\N
6	18	2011-05-28 16:14:54.481249	\N
9	31	2011-05-28 16:20:35.761357	\N
9	4	2011-05-28 16:20:38.703559	\N
\.


--
-- Name: uses_pkey; Type: CONSTRAINT; Schema: public; Owner: esgcet; Tablespace: 
--

ALTER TABLE ONLY uses
    ADD CONSTRAINT uses_pkey PRIMARY KEY (idproject, idserviceinstance, startdate);


--
-- Name: uses_idserviceinstance_fkey; Type: FK CONSTRAINT; Schema: public; Owner: esgcet
--

ALTER TABLE ONLY uses
    ADD CONSTRAINT uses_idserviceinstance_fkey FOREIGN KEY (idserviceinstance) REFERENCES service_instance(id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--

