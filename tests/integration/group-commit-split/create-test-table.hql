USE '/';
DROP TABLE IF EXISTS LoadTest;
CREATE TABLE LoadTest ( Field ) GROUP_COMMIT_INTERVAL=100;
quit;
