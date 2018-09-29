import json
from block import block

class BlockInfoEncoder(json.JSONEncoder):
  def default(self, o):
    if isinstance(o, block.info):
      return { 'color' : o.color, 'width' : o.width }
    return super(BlockInfoEncoder, self).default(o)

def post(blocks):
  print(json.dumps({ "orders" : blocks }, cls = BlockInfoEncoder))
