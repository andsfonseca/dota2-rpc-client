import { bootstrapApplication } from '@angular/platform-browser';
import { enableProdMode, provideZoneChangeDetection } from '@angular/core';

import { IndexComponent } from './app/ui/index/index.component';
import { environment } from './environments/environment';

if (environment.production) {
  enableProdMode();
}

bootstrapApplication(IndexComponent, {
  providers: [
    provideZoneChangeDetection({ eventCoalescing: true })
  ]
}).catch(err => console.error(err));
