#
# Copyright (c) 2015 Cossack Labs Limited
#
# Licensed under the Apache License, Version 2.0 (the "License")
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

"""
store encrypted data in postgreSQL tables
we assume that table "scell_data" must contain data, length of which must be
not more then plain data
for store additional encryption data we will use separate table "scell_data_auth"
stored object is represented by two independent string value
"""

import base64
import sys
import psycopg2
import psycopg2.extras
from pythemis import scell

master_key = base64.b64decode(b'c2NlbGxfeG1sX2Zvcm1hdC1wcmVzZXJ2aW5nX2VuY3J5cHRpb24ucHk=')

CREATE_SCELL_DATA_TABLE_SQL = ("CREATE TABLE IF NOT EXISTS scell_data ("
                               "id serial PRIMARY KEY, num bytea, data bytea);")
CREATE_SCELL_DATA_AUTH_TABLE_SQL = (
    "CREATE TABLE IF NOT EXISTS scell_data_auth ("
    "id serial PRIMARY KEY, num bytea, data bytea);")


def init_table(connection):
    with connection.cursor() as cursor:
        cursor.execute(CREATE_SCELL_DATA_TABLE_SQL)
        cursor.execute(CREATE_SCELL_DATA_AUTH_TABLE_SQL)
        connection.commit()


def add_record(connection, field1, field2):
    encryptor = scell.SCellTokenProtect(master_key)
    # encrypt field1
    encrypted_field1, field1_auth_data = encryptor.encrypt(
        field1.encode('utf-8'))
    # encrypt field2
    encrypted_field2, field2_auth_data = encryptor.encrypt(
        field2.encode('utf8'))

    with connection.cursor() as cursor:
        # store main cryptomessage
        cursor.execute(
            "INSERT INTO scell_data (num, data) VALUES (%s, %s) RETURNING ID",
            (psycopg2.Binary(encrypted_field1),
             psycopg2.Binary(encrypted_field2)))
        new_id_value = cursor.fetchone()[0]

        # store additional auth values
        cursor.execute(
            "INSERT INTO scell_data_auth (id, num, data) VALUES (%s, %s, %s)",
            (new_id_value,
             psycopg2.Binary(field1_auth_data),
             psycopg2.Binary(field2_auth_data)))
        connection.commit()
    return new_id_value


def get_record(connection, id):
    # retrieve record from db by id
    dec = scell.SCellTokenProtect(master_key)
    with connection.cursor() as cursor:
        cursor.execute(
            "SELECT * FROM scell_data "
            "INNER JOIN scell_data_auth ON "
            "scell_data.id = %s AND scell_data.id=scell_data_auth.id;",
            [id]
        )
        row = cursor.fetchone()
        _, DATA_NUM, DATA_DATA, AUTH_ID, AUTH_NUM, AUTH_DATA = range(6)
        if (sys.version_info > (3, 0)):
            print("stored data:",
                  row[DATA_NUM].tobytes(), row[AUTH_NUM].tobytes(),
                  row[DATA_DATA].tobytes(), row[AUTH_DATA].tobytes())
        else:
            print("stored data:",
                  bytes(row[DATA_NUM]), bytes(row[AUTH_NUM]),
                  bytes(row[DATA_DATA]), bytes(row[AUTH_DATA]))
        num = dec.decrypt(bytes(row[DATA_NUM]),
                          bytes(row[AUTH_NUM])).decode('utf-8')

        data = dec.decrypt(bytes(row[DATA_DATA]),
                           bytes(row[AUTH_DATA])).decode('utf-8')
    return num, data


if __name__ == '__main__':
    dsn = ("dbname=scell_token_protect_test user=postgres password=postgres "
           "host=localhost")
    with psycopg2.connect(dsn) as connection:
        init_table(connection)
        row_id = add_record(connection, "First record", "Second record")
        record = get_record(connection, row_id)
        print("real_data: ", record)
