--
-- Drop the dashboard_queue table
--

SET search_path = esgf_dashboard, pg_catalog;

drop table if exists esgf_dashboard.dashboard_queue;
--
-- Drop the functions used for updating the dashboard_queue;
--
drop function if exists store_dashboard_queue() CASCADE;
drop function if exists update_dashboard_queue() CASCADE;
drop function if exists delete_dashboard_queue() CASCADE;

