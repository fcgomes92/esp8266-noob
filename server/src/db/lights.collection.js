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
      isEffectActive: {
        type: 'boolean',
        default: true,
      },
      selectedEffect: {
        type: 'number',
        default: 1,
      },
      pixels: {
        type: 'array',
        default: [],
        items: {
          type: 'string',
        },
      },
    },
    indexes: ['id'],
    required: ['id'],
  },
};
