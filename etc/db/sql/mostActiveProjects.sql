DROP PROCEDURE IF EXISTS `mostActiveProjects`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `mostActiveProjects`(IN start_date DATETIME, IN end_date DATETIME, IN lb INTEGER, IN size INTEGER, IN sortField VARCHAR(20), IN ascendant BOOLEAN, IN id_user INTEGER)
BEGIN
        DECLARE dir CHAR(4);
        IF ascendant THEN
                SET dir="ASC";
        ELSE
                SET dir="DESC";
        END IF;
        SET @A = CONCAT("SELECT p.id, p.name, p.startDate, p.endDate, AVG(percentage) AS average
        FROM (
                SELECT u.idProject, 100*
                (
                        SELECT COUNT(*)
                        FROM service_status
                        WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN '", start_date, "' AND '", end_date,
                "') / (
                        SELECT COUNT(*)
                        FROM service_status
                        WHERE idServiceInstance=s.id AND timestamp BETWEEN '", start_date, "' AND '", end_date,
                "') AS percentage
                FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id INNER JOIN `join` j ON j.idProject=u.idProject
                WHERE u.endDate IS NULL AND j.idUser=", id_user, "
        ) t INNER JOIN project p ON p.id=t.idProject
        GROUP BY p.id
        ORDER BY ", sortField, " ", dir, "
        LIMIT ", lb, " , ", size);
        PREPARE STMT FROM @A;
        EXECUTE STMT;
        DEALLOCATE PREPARE STMT;
END
$$