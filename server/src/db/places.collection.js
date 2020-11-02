export default {
  name: 'places',
  schema: {
    version: 0,
    type: 'object',
    properties: {
      name: {
        type: 'string',
        primary: true,
      },
    },
    indexes: ['name'],
    required: ['name'],
  },
};
