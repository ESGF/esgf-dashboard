DROP PROCEDURE IF EXISTS `hostActivity1`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `hostActivity1`(IN project_id INTEGER, IN host_id INTEGER, IN start_date DATETIME, IN end_date DATETIME, IN lb INTEGER, IN size INTEGER)
BEGIN
        IF project_id IS NULL THEN
                SET @A = CONCAT("SELECT s.id, s.name, 100*
                                                    (
                                                        SELECT COUNT(*)
                                                        FROM service_status
                                                        WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN '", start_date, "' AND '", end_date,"'
                                                    ) / (
                                                        SELECT COUNT(*)
                                                        FROM service_status
                                                        WHERE idServiceInstance=s.id AND timestamp BETWEEN '", start_date, "' AND '", end_date,"'
                                                    ) AS percentage
                                                FROM service_instance s
                                                WHERE s.idHost=", host_id, "
                                                ORDER BY s.name
                                                LIMIT ", lb, ", ", size, ";");
        ELSE
                SET @A = CONCAT("SELECT s.id, s.name, 100*
                                                    (
                                                        SELECT COUNT(*)
                                                        FROM service_status
                                                        WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN '", start_date, "' AND '", end_date,"'
                                                    ) / (
                                                        SELECT COUNT(*)
                                                        FROM service_status
                                                        WHERE idServiceInstance=s.id AND timestamp BETWEEN '", start_date, "' AND '", end_date,"'
                                                    ) AS percentage
                                                FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id
                                                WHERE s.idHost=", host_id, " AND u.idProject=", project_id, " AND u.endDate IS NULL
                                                ORDER BY s.name
                                                LIMIT ", lb, ", ", size, ";");
        END IF;
        PREPARE STMT FROM @A;
        EXECUTE STMT;
        DEALLOCATE PREPARE STMT;
END
$$