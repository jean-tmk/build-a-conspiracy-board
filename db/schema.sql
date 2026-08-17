CREATE TABLE cases(case_id INTEGER PRIMARY KEY,title TEXT NOT NULL,opened_at TEXT DEFAULT CURRENT_TIMESTAMP,status TEXT CHECK(status IN('open','cold','solved')) DEFAULT 'open');
CREATE TABLE evidence(evidence_id INTEGER PRIMARY KEY,case_id INTEGER NOT NULL REFERENCES cases(case_id),title TEXT NOT NULL,detail TEXT,evidence_type TEXT,confidence REAL CHECK(confidence BETWEEN 0 AND 1),x REAL,y REAL);
CREATE TABLE connections(connection_id INTEGER PRIMARY KEY,case_id INTEGER NOT NULL REFERENCES cases(case_id),source_id INTEGER NOT NULL REFERENCES evidence(evidence_id),target_id INTEGER NOT NULL REFERENCES evidence(evidence_id),label TEXT,weight REAL DEFAULT 1,verified INTEGER DEFAULT 0,UNIQUE(case_id,source_id,target_id));
CREATE INDEX evidence_case_idx ON evidence(case_id);
CREATE INDEX connections_case_idx ON connections(case_id);
