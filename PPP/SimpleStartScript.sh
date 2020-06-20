#!/bin/bash

cd build-SimplePirate-Desktop_Qt_5_9_4_GCC_64bit-Debug
./SimplePirate  queue&
./SimplePirate  server&
./SimplePirate  server&
./SimplePirate  server&
./SimplePirate  server&
./SimplePirate  client&
#./SimplePirate  client&
#./SimplePirate  client&
#./SimplePirate  client&
#./SimplePirate  client&
#./SimplePirate  client&
#./SimplePirate  client&
#./SimplePirate  client&
#./SimplePirate  client&
./SimplePirate  client
