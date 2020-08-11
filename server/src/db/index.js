import PouchAdapterHttp from 'pouchdb-adapter-http';
import PouchAdapterMemory from 'pouchdb-adapter-memory';
import { addRxPlugin, createRxDatabase } from 'rxdb';
import lightsCollection from './lights.collection';

addRxPlugin(PouchAdapterMemory);
addRxPlugin(PouchAdapterHttp);

let db;

export const getDatabaseClient = async () => {
  if (db) return db;
  try {
    db = await createRxDatabase({
      name: process.env.RXDB_DATABASE_NAME,
      adapter: 'memory',
      password: process.env.RXDB_PASSWORD,
      multiInstance: true,
      eventReduce: false,
    });
    const collections = await Promise.all([
      await db.collection(lightsCollection),
    ]);
    await Promise.all(
      collections.map(async col =>
        col.sync({
          remote: process.env.POUCH_DB_SERVER,
        })
      )
    );
    return db;
  } catch (error) {
    console.log(error);
    return null;
  }
};

export default db;
