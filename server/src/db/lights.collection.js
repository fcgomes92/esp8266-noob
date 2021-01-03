export default {
  name: 'lights',
  schema: {
    version: 0,
    type: 'object',
    properties: {
      id: {
        type: 'string',
        primary: true,
      },
      path: {
        type: 'string',
        default: '',
      },
      effect: {
        type: 'number',
        default: 1,
      },
      color: {
        type: 'string',
        default: 1,
      },
      speed: {
        type: 'number',
        default: 1,
      },
    },
    indexes: ['id'],
    required: ['id'],
  },
};
