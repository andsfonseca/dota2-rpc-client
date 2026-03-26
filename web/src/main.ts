import { bootstrapApplication } from '@angular/platform-browser';
import { enableProdMode, provideZoneChangeDetection } from '@angular/core';

import { IndexComponent } from './app/ui/index/index.component';
import { environment } from './environments/environment';

import { provideHighlightOptions } from 'ngx-highlightjs';

if (environment.production) {
  enableProdMode();
}

bootstrapApplication(IndexComponent, {
  providers: [
    provideZoneChangeDetection({ eventCoalescing: true }),

    provideHighlightOptions({
      coreLibraryLoader: () => import('highlight.js/lib/core'),
      languages: {
        bash: () => import('highlight.js/lib/languages/bash')
      }
    })
  ]
}).catch(err => console.error(err));